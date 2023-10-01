#include <iostream>
#include <numeric>

#include "backtest.hpp"
#include "../runtime.hpp"

using namespace backtest;
using namespace backtest::util;
using namespace backtest::env;

using namespace runtime;
using namespace runtime::param;

/**
 * Refresh indicator variables for each date and code.
 * @param {string&} date: target date of year_data_cache. ex) "20230926"
 * @param {string&} code: target stock code to get data in above date.
 * @return -1: Can not find date, try to next date
 * @return -2: Can not find code, try to other code
 * @return 0: success
*/
int backtest::refresh(string& date, string& code) { // TODO: rewrite the return type to int with error code, find date error and find stock error, date error -> break the loop, stock error -> continue the loop
    tm tm;
    strptime(date.c_str(), "%Y%m%d", &tm);
    if(tm.tm_year + 1900 != data_year) {
        data_year = tm.tm_year + 1900;
        load_data("./src/data/datas/krx_" + to_string(tm.tm_year + 1900) + ".txt");
    }
    
    if(year_data_cache.find(date) == year_data_cache.end())
        return -1;
    
    boost::json::object day_data = year_data_cache[date].as_object();
    if(day_data.find(code) == day_data.end())
        return -2;
    
    boost::json::array day_stock = day_data[code].as_array();
    indicator::PRICE = day_stock[0].as_int64();
    indicator::PER = day_stock[1].as_double();
    indicator::PBR = day_stock[2].as_double();
    indicator::AVG_5 = day_stock[3].as_double();
    indicator::AVG_20 = day_stock[4].as_double();
    indicator::AVG_60 = day_stock[5].as_double();
    indicator::RSI_9 = day_stock[6].as_double();
    indicator::RSI_14 = day_stock[7].as_double();
    indicator::RSI_28 = day_stock[8].as_double();
    indicator::BOLLINGER_LOW = day_stock[9].as_double();
    indicator::BOLLINGER_HIGH = day_stock[10].as_double();

    return 0;
}

/**
 * 
*/
void backtest::run(string& start, string& end, int init_cash, vector<string>& target_list) {
    tm tm_today;
    tm tm_end;
    strptime(end.c_str(), "%Y%m%d", &tm_end);
    strptime(start.c_str(), "%Y%m%d", &tm_today);

    cash = init_cash;
    total_evaluate = cash;
    
    char today_format[16];
    map<string, vector<float>> stock_scores; // {{code, price} score}, score must be +
    map<int, pair<string, int>, greater<int>> buy_list;
    vector<string> sell_list;

    for(; !tm_is_equal(tm_today, tm_end); tm_inc(tm_today)) {
        strftime(today_format, sizeof(today_format), "%Y%m%d", &tm_today);
        date = string(today_format);
        cout << "Date: " + to_string(tm_today.tm_year + 1900) + "/" + to_string(tm_today.tm_mon + 1) + "/" + to_string(tm_today.tm_mday) << endl;
        if(tm_today.tm_wday == 0 || tm_today.tm_wday == 6) {
            cout << "[ Today is " + string((tm_today.tm_wday == 0) ? "Sunday" : "Saturday") + ". ]" << endl;
            continue;
        }

        // refresh data for each stock and decide to action with score.
        for(auto& code : target_list) {
            int ret = refresh(date, code);
            if(ret == -1)
                break;
            else if(ret == -2)
                continue;
            
            vector<float> scores = strategy::v0();
            stock_scores[code] = scores;
            
            int score = (int) accumulate(scores.begin(), scores.end(), 0);
            if(score > 0)
                buy_list[score] = {code, indicator::PRICE};
            
            if(account.find(code) != account.end()) {
                if(score < 0
                    || indicator::PRICE < (float) account[code][1] * (1 - LOSS_CUT)
                    || indicator::PRICE > (float) account[code][1] * (1 + PROFIT_CUT))
                    sell_list.push_back(code);
                account[code][2] = indicator::PRICE;
            }
        }

        // Print account with price
        refresh_account();
        buysell(buy_list, sell_list, stock_scores);
        show_account();
    }

    cout << "----- [LAST STATUS] -----" << endl;
    for(auto it : tr_log) {
        cout << it << endl;
    }
    clear_account();
    refresh_account();
    show_account();
}