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
    char one_year_past[10];
    strptime(date.c_str(), "%Y%m%d", &tm);
    tm.tm_year -= 1;
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

        indicator::OPEN = &cache[code].data[0];
        indicator::HIGH = &cache[code].data[1];
        indicator::LOW = &cache[code].data[2];
        indicator::CLOSE = &cache[code].data[3];
        indicator::VOLUME = &cache[code].data[4];
        indicator::CHANGE = &cache[code].data[5];
        indicator::PER = &cache[code].data[6];
        indicator::PBR = &cache[code].data[7];
    }
    sqlite3_finalize(stmt);
    return success;
}

/**
 * 
*/
void backtest::run(string& start, string& end, int init_cash, vector<string>& target_list) {
    DB::SQL_OPEN();
    DB::SQL_CACHING(start, end, target_list);
        
    cash = init_cash;
    total_evaluate = cash;
    
    map<string, vector<float>> stock_scores; // {{code, price} score}, score must be +
    map<int, pair<string, int>, greater<int>> buy_list;
    vector<string> sell_list;
    
    char today_format[16];
    string today;
    tm tm_today;
    tm tm_end;
    strptime(end.c_str(), "%Y%m%d", &tm_end);
    strptime(start.c_str(), "%Y%m%d", &tm_today);

    for(; !tm_is_equal(tm_today, tm_end); tm_inc(tm_today)) {
        strftime(today_format, sizeof(today_format), "%Y%m%d", &tm_today);
        today = string(today_format);
        cout << "Date: " + to_string(tm_today.tm_year + 1900) + "/" + to_string(tm_today.tm_mon + 1) + "/" + to_string(tm_today.tm_mday) << endl;
        if(tm_today.tm_wday == 0 || tm_today.tm_wday == 6) {
            cout << "[ Today is " + string((tm_today.tm_wday == 0) ? "Sunday" : "Saturday") + ". ]" << endl;
            continue;
        }

        // refresh data for each stock and decide to action with score.
        for(auto& code : target_list) {
            bool ret = refresh(today, code);
            if( !ret )
                continue;
            
            vector<float> scores = strategy::v0(code);
            stock_scores[code] = scores;
            
            int score = (int) accumulate(scores.begin(), scores.end(), 0);
            int price = indicator::CLOSE->operator()(runtime::cache[code].last_idx);
            if(score > 0)
                buy_list[score] = {code, price};
            
            if(account.find(code) != account.end()) {
                if(score < 0
                    || price < (float) account[code][1] * (1 - LOSS_CUT)
                    || price > (float) account[code][1] * (1 + PROFIT_CUT))
                    sell_list.push_back(code);
                account[code][2] = price;
            }
        }

        // Print account with price
        refresh_account();
        buysell(today, buy_list, sell_list, stock_scores);
        show_account();
    }

    cout << "----- [LAST STATUS] -----" << endl;
    for(auto it : tr_log) {
        cout << it << endl;
    }
    clear_account();
    refresh_account();
    show_account();

    printf("Database closing..\n");
    sqlite3_close(db);
}