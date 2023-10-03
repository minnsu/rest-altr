#include <iostream>
#include <numeric>

#include "backtest.hpp"
#include "../runtime.hpp"

using namespace backtest;
using namespace backtest::util;
using namespace backtest::env;

using namespace runtime;
using namespace runtime::DB;
using namespace runtime::param;

/**
 * Refresh indicator variables for each date and code.
 * @param {string&} date: target date of year_data_cache. ex) "20230926"
 * @param {string&} code: target stock code to get data in above date.
*/
bool backtest::refresh(string& date, string& code) {
    sqlite3_stmt *stmt;
    string market;
    int rc = sqlite3_prepare_v2(db,
        ("SELECT market FROM STOCK_INFO WHERE code='" + code + "';").c_str(),
        -1, &stmt, NULL
    ); SQL_ERROR_CHECK(rc, NULL);
    
    if(sqlite3_step(stmt) == SQLITE_ROW) {
        market = string((const char*) sqlite3_column_text(stmt, 0));
    } else
        return false;
    sqlite3_finalize(stmt);

    tm tm;
    strptime(date.c_str(), "%Y%m%d", &tm);
    tm.tm_year -= 1;
    
    char one_year_past[10];
    strftime(one_year_past, sizeof(one_year_past), "%Y%m%d", &tm);
    
    rc = sqlite3_prepare_v2(db,
        ("SELECT open, high, low, close, volume, change, per, pbr FROM " + string(market) +
        " WHERE code='" + code + "' AND date ='" + date + "';").c_str(), -1, &stmt, NULL
    ); SQL_ERROR_CHECK(rc, NULL);
    
    bool success = false;
    if(sqlite3_step(stmt) == SQLITE_ROW) {
        success = true;

        int idx = cache[code].last_idx + 1;
        cache[code].data[0](idx) = sqlite3_column_double(stmt, 0); 
        cache[code].data[1](idx) = sqlite3_column_double(stmt, 1);
        cache[code].data[2](idx) = sqlite3_column_double(stmt, 2);
        cache[code].data[3](idx) = sqlite3_column_double(stmt, 3);
        cache[code].data[4](idx) = sqlite3_column_double(stmt, 4);
        cache[code].data[5](idx) = sqlite3_column_double(stmt, 5);
        cache[code].data[6](idx) = sqlite3_column_double(stmt, 6);
        cache[code].data[7](idx) = sqlite3_column_double(stmt, 7);
        cache[code].last_idx++;
        cache[code].last_date = date;

        indicator::OPEN = &cache[code].data[0];
        indicator::HIGH = &cache[code].data[1];
        indicator::LOW = &cache[code].data[2];
        indicator::CLOSE = &cache[code].data[3];
        indicator::VOLUME = &cache[code].data[4];
        indicator::CHANGE = &cache[code].data[5];
        indicator::PER = &cache[code].data[6];
        indicator::PBR = &cache[code].data[7];
    }
    if(!success)
        return success;
    sqlite3_finalize(stmt);

    indicator::AVG_5 = series::rolling_mean(*indicator::CLOSE, 5, 0);
    indicator::AVG_20 = series::rolling_mean(*indicator::CLOSE, 20, 0);
    indicator::AVG_60 = series::rolling_mean(*indicator::CLOSE, 60, 0);
    
    Series std_dev = series::rolling_std(*indicator::CLOSE, 20, 0);
    indicator::BOLLINGER_LOW = indicator::AVG_20 - 2 * std_dev;
    indicator::BOLLINGER_HIGH = indicator::AVG_20 + 2 * std_dev;

    indicator::RSI_9 = series::RSI(*indicator::CHANGE, 9);
    indicator::RSI_14 = series::RSI(*indicator::CHANGE, 14);
    indicator::RSI_28 = series::RSI(*indicator::CHANGE, 28);
    
    return success;
}

/**
 * Backtesting running code.
 * @param {string&} start: backtesting start date
 * @param {string&} end: backtesting end date
 * @param {int} init_cash: initial cash
 * @param {vector<string>&} target_list: target stock code list
*/
void backtest::run(string& start, string& end, int init_cash, vector<string>& target_list) {
    DB::SQL_OPEN();
    DB::SQL_CACHING(start, end, target_list);
        
    cash = init_cash;
    total_evaluate = cash;
    
    multimap<float, pair<string, int>> scored_list;
    
    tm tm_today;
    strptime(start.c_str(), "%Y%m%d", &tm_today);
    tm tm_end;
    strptime(end.c_str(), "%Y%m%d", &tm_end);

    for(; !tm_is_equal(tm_today, tm_end); tm_inc(tm_today)) {
        cout << "Date: " + to_string(tm_today.tm_year + 1900) + "/" + to_string(tm_today.tm_mon + 1) + "/" + to_string(tm_today.tm_mday) << endl;
        if(isWeekend(tm_today))
            continue;
        string today = tm_to_string(tm_today);

        // refresh data for each stock and decide to action with score.
        scored_list.clear();
        for(auto& code : target_list) {
            bool ret = refresh(today, code);
            if( !ret )
                continue;
            
            float score = strategy(code);
            int price = indicator::CLOSE->operator()(runtime::cache[code].last_idx);
            scored_list.insert({score, {code, price}});
            if(account.find(code) != account.end())
                account[code][2] = price;
        }

        buysell(today, scored_list);
        // show_account();
    }

    cout << "----- [LAST STATUS] -----" << endl;
    for(auto& it : tr_log) {
        cout << it << endl;
    }
    clear_account();
    show_account();

    printf("Database closing..\n");
    sqlite3_close(db);
}