#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <numeric>

#include <boost/json/parse.hpp>

#include "backtest.hpp"
#include "../runtime.hpp"

using namespace backtest;
using namespace backtest::SQL;
using namespace backtest::env;

using namespace runtime::param;

/**
 * Check if tm variable indicate same day.
 * @param {tm&} day1: day 1.
 * @param {tm&} day2: day 2.
*/
bool util::tm_is_equal(tm& day1, tm& day2) {
    return day1.tm_year == day2.tm_year
            && day1.tm_mon == day2.tm_mon
            && day1.tm_mday == day2.tm_mday;
}

/**
 * Increment tm type variable for 1 day.
 * @param {tm&} day: target to increment.
*/
void util::tm_inc(tm& day) {
    day.tm_mday += 1;
    day.tm_wday = (day.tm_wday + 1) % 7;
    switch(day.tm_mon) {
    case 0:
    case 2:
    case 4:
    case 6:
    case 7:
    case 9:
    case 11:
        if(day.tm_mday > 31) {
            day.tm_mon += 1;
            day.tm_mday = 1;
        }
        break;
    case 1:
        if(day.tm_year % 4 == 0 && day.tm_mday > 29) {
            day.tm_mon += 1;
            day.tm_mday = 1;
        } else if(day.tm_year > 28) {
            day.tm_mon += 1;
            day.tm_mday = 1;
        }
        break;
    default:
        if(day.tm_mday > 30) {
            day.tm_mon += 1;
            day.tm_mday = 1;
        }
    }
    if(day.tm_mon >= 12) {
        day.tm_year += 1;
        day.tm_mon = 0;
    }
}

/**
 * Backtesting only. Buy or sell stocks by specified conditions.
 * @param {map<string, array<int, 3>>&} account: current stock account
 * @param {int&} total_evaluate: current total evaluation
 * @param {int&} cash: current cash
 * @param {vector<pair<string, pair<int, int>>>&} buy_list: buy targets
 * @param {vector<string>&} sell_list: sell targets
*/
void util::buysell(string& date, map<int, pair<string, int>, greater<int>>& buy_list, vector<string>& sell_list, map<string, vector<float>>& scores) {
    char log[256];
    int qty, avg_price, cur_price, profit_loss;
    float profit_loss_rate;

    // First, sell if I have the stock in sell list.
    for(auto& code : sell_list) {
        qty = account[code][0];
        avg_price = account[code][1];
        cur_price = account[code][2];
        
        profit_loss_rate = 100 * (float) cur_price / avg_price - 100;
        profit_loss = (cur_price - avg_price) * qty;

        memset(log, 0, sizeof(log));
        sprintf(log, "[%s] 매도 [%s] 평균가: %d\t->\t현재가: %d\t보유량: %d\t => 총 매도가: %d\t(손익률: %.2f%%\t손익: %d)",
            date.c_str(), code.c_str(),
            avg_price, cur_price, qty, cur_price * qty,
            profit_loss_rate, profit_loss);
        int sum_scores = accumulate(scores[code].begin(), scores[code].end(), 0);
        tr_log.push_back(string(log) + " score at sell: " + to_string(sum_scores));

        cash += ((float) (1 - TAX - CHARGE) * qty * cur_price);
        account.erase(code);
    }
    sell_list.clear();
    // Second, buy
    for(auto& it : buy_list) {
        cur_price = it.second.second;
        qty = (total_evaluate * MAX_DIST_RATE) / cur_price;
        int required_money = (float) (1 + CHARGE) * cur_price * qty;
        
        if(cash < required_money) {
            qty = cash / cur_price;
            required_money = (float) (1 + CHARGE) * cur_price * qty;
            if(qty == 0 || cash < required_money)
                break;
        }

        string code = it.second.first;
        auto it2 = account.find(code);
        if(it2 != account.end()) {
            int have_qty = account[code][0];
            avg_price = account[code][1];

            // No exceed the double of MAX_DIST_RATE
            if(have_qty * avg_price + required_money > total_evaluate * MAX_DIST_RATE * 2)
                continue;
            
            account[code][1] = (account[code][1] * have_qty + required_money) / (have_qty + qty);
            account[code][0] += qty;
        } else
            account[code] = {qty, cur_price, cur_price};
        
        memset(log, 0, sizeof(log));
        sprintf(log, "[%s] 매수 [%s] 현재가: %d\t수량: %d\t => 총 매수가: %d",
            date.c_str(), code.c_str(),
            cur_price, qty, cur_price * qty);
        tr_log.push_back(string(log));
        cash -= required_money;
    }
    buy_list.clear();
}

/**
 * Refresh backtest::env::total_evaluate.
*/
void backtest::util::refresh_account() {
    total_evaluate = cash;
    for(auto it : account)
        total_evaluate += it.second[0] * it.second[2];
}

/**
 * Show backtest::env::account.
*/
void backtest::util::show_account() {
    char log[256];
    string code;
    int qty, avg_price, cur_price, profit_loss;
    float profit_loss_rate;

    cout << "┌----- 잔고 -----------------------------------------------------------------------------------------------------------┐" << endl;
    for(auto& it : account) {
        code = it.first;
        qty = it.second[0];
        avg_price = it.second[1];
        cur_price = it.second[2];

        profit_loss_rate = 100 * (float) cur_price / avg_price - 100;
        profit_loss = (cur_price - avg_price) * qty;
        memset(log, 0, sizeof(log));
        sprintf(log, "| [%s] - 평균가: %d\t현재가: %d\t보유량: %d\t => 평가액: %d\t(손익률: %.2f%%\t손익: %d)",
            code.c_str(), avg_price, cur_price, qty, cur_price * qty, profit_loss_rate, profit_loss);
        printf("%s\n", log);
    }
    cout << "| 현금: " + to_string(cash) << endl;
    cout << "| 총 평가액: " + to_string(total_evaluate) << endl;
    cout << "└----------------------------------------------------------------------------------------------------------------------┘" << endl;
}

/**
 * Clear backtest::env::account.
*/
void backtest::util::clear_account() {
    for(auto it : account) {
        cash += (float) (1 - TAX - CHARGE) * it.second[2] * it.second[0];
    }
    account.clear();
}





/**
 * 
*/
void backtest::SQL::SQL_ERROR_CHECK(int rc, char *ErrMsg) {
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", ErrMsg);
        sqlite3_free(ErrMsg);
        sqlite3_close(db);
        exit(1);
    }
}

/**
 * 
*/
int backtest::SQL::callback_code2market(void *market, int argc, char **argv, char **col_name) {
    strcpy((char *) market, argv[0]);
    return 0;
}