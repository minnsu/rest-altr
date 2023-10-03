#include <ctime>
#include <iostream>
#include <set>

#include <boost/json/parse.hpp>

#include "backtest.hpp"
#include "../runtime.hpp"

using namespace backtest;
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
 * Transform tm to string by format.
 * @param {tm&} tm: target to transform to string
 * @param {string} format: format like printf, default="%Y%m%d"
*/
string backtest::util::tm_to_string(tm& tm, string format) {
    char tmp[16];
    strftime(tmp, sizeof(tmp), format.c_str(), &tm);
    return string(tmp);
}

/**
 * Check if parameter today is saturday or sunday.
 * @param {tm&} today: target to check if weekend
*/
bool backtest::util::isWeekend(tm& today) {
    if(today.tm_wday == 0 || today.tm_wday == 6) {
        // cout << "[ Today is " + string((today.tm_wday == 0) ? "Sunday" : "Saturday") + ". ]" << endl;
        return true;
    }
    return false;
}


/**
 * Backtesting only. Buy or sell stocks by specified conditions.
 * @param {string&} date: for record log.
 * @param {multimap<float, pair<string, int>>&} scored_list: sorted by score list. {score, {code, price}}
*/
void util::buysell(string& date, multimap<float, pair<string, int>>& scored_list) {
    refresh_account();
    char log[256];
    
    string code;
    float score;
    int qty, price, avg_price;
    
    int profit_loss;
    float profit_loss_rate;

    // Sell
    set<string> sell_list;
    for(auto& it : scored_list) {
        score = it.first;
        code = it.second.first;
        price = it.second.second;

        if(account.find(code) != account.end()) {
            qty = account[code][0];
            avg_price = account[code][1];

            profit_loss_rate = 100 * (float) price / avg_price - 100;
            profit_loss = (price - avg_price) * qty;

            string sell_flag = "None";
            if(score < 0)
                sell_flag = "SCORE LOWER THAN 0";
            else if(profit_loss_rate / 100 > PROFIT_CUT)
                sell_flag = "EXCEED PROFIT CUT";
            else if(profit_loss_rate / 100 < -LOSS_CUT)
                sell_flag = "EXCEED LOSS CUT";
            else
                continue;
            
            memset(log, 0, sizeof(log));
            sprintf(log, "[%s] SELL [%s] Avg: %-8d -> Price: %-8d Qty: %-10d => Total: %10d ---------- (PL Rate: %.2f%% PL: %5d) (Score: %.4f Reason: %s)",
                date.c_str(), code.c_str(),
                avg_price, price, qty, price * qty,
                profit_loss_rate, profit_loss, score, sell_flag.c_str());
            
            tr_log.push_back(log);
            
            cash += ((float) (1 - TAX - CHARGE) * qty * price);
            account.erase(code);

            sell_list.insert(code);
        }
    }

    // Buy
    for(auto it = scored_list.rbegin(); it != scored_list.rend(); it++) {
        score = it->first;
        code = it->second.first;
        price = it->second.second;

        if(score <= 0 || sell_list.find(code) != sell_list.end())
            break;
        
        qty = (total_evaluate * MAX_DIST_RATE) / price;
        int required_money = (float) (1 + CHARGE) * price * qty;
        if(cash < required_money) {
            qty = cash / price;
            required_money = (float) (1 + CHARGE) * price * qty;
            if(qty == 0 || cash < required_money)
                break;
        }

        if(account.find(code) != account.end()) {
            int have_qty = account[code][0];
            avg_price = account[code][1];

            // No exceed the double of MAX_DIST_RATE
            if(have_qty * avg_price + required_money > total_evaluate * MAX_DIST_RATE * 2)
                continue;
            
            account[code][1] = (account[code][1] * have_qty + required_money) / (have_qty + qty);
            account[code][0] += qty;
        } else
            account[code] = {qty, price, price};
        
        memset(log, 0, sizeof(log));
        sprintf(log, "[%s] BUY  [%s] Price: %-8d Qty: %-10d => Total: %10d ---------- (Score: %.4f)",
            date.c_str(), code.c_str(),
            price, qty, price * qty, score);
        tr_log.push_back(string(log));
        cash -= required_money;
    }
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
    refresh_account();
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
        sprintf(log, "| [%s] - 평균가: %-8d 현재가: %-8d 보유량: %-10d => 평가액: %-10d (손익률: %3.2f%% 손익: %10d)",
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