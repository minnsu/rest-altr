#include <iostream>
#include <numeric>

#include "backtest.hpp"
#include "../runtime.hpp"

using namespace backtest;
using namespace backtest::util;
using namespace backtest::SQL;
using namespace backtest::env;

using namespace runtime;
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
        " WHERE code='" + code + "' AND date > '" + one_year_past + "' AND date < '" + date + "';").c_str(), -1, &stmt, NULL
    ); SQL_ERROR_CHECK(rc, NULL);
    
    bool success = false;
    int idx;
    for(idx = 0; sqlite3_step(stmt) == SQLITE_ROW; idx++) { // Too slow.
        success = true;
        indicator::OPEN(idx) = sqlite3_column_double(stmt, 0);
        indicator::HIGH(idx) = sqlite3_column_double(stmt, 1);
        indicator::LOW(idx) = sqlite3_column_double(stmt, 2);
        indicator::CLOSE(idx) = sqlite3_column_double(stmt, 3);
        indicator::VOLUME(idx) = sqlite3_column_double(stmt, 4);
        indicator::PER(idx) = sqlite3_column_double(stmt, 6);
        indicator::PBR(idx) = sqlite3_column_double(stmt, 7);
    }
    sqlite3_finalize(stmt);
    return success;
}

/**
 * 
*/
void backtest::run(string& start, string& end, int init_cash, vector<string>& target_list) {
    int rc = sqlite3_open("./db/stocks.db", &db);
    if(rc != SQLITE_OK) {
        cout << "Can not open database: \n" << sqlite3_errmsg(db);
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_exec(db, "PRAGMA cache_size=16384;", NULL, NULL, NULL);
    sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
        
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
            
            vector<float> scores = strategy::v0();
            stock_scores[code] = scores;
            
            int score = (int) accumulate(scores.begin(), scores.end(), 0);
            // if(score > 0)
            //     buy_list[score] = {code, indicator::CLOSE};
            
            // if(account.find(code) != account.end()) {
            //     if(score < 0
            //         || indicator::CLOSE < (float) account[code][1] * (1 - LOSS_CUT)
            //         || indicator::CLOSE > (float) account[code][1] * (1 + PROFIT_CUT))
            //         sell_list.push_back(code);
            //     account[code][2] = indicator::CLOSE;
            // }
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

    sqlite3_close(db);
}