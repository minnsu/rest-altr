#include <fstream>
#include <iostream>
#include <sstream>
#include <numeric>

#include "engine.hpp"

int cur_year = 0;
boost::json::object backtest::data_cache;

float backtest::tax = 0.002;
float backtest::charge = 0.00015;
float backtest::max_distribute_rate = 0.25;
int backtest::cash = 0;
int backtest::total_evaluate = 0;
float backtest::loss_cut = 0.03;
float backtest::profit_cut = 0.1;
map<string, array<int, 3>> backtest::account;

string backtest::date;
vector<string> backtest::tr_log;

/**
 * Load data from path as boost::json::object to backtest::data_cache
 * @param {string} path: path of data file.
*/
void backtest::utils::load_data(string path) {
    ifstream fs(path);
    if(!fs.is_open()) {
        perror("File not found!");
        exit(1);
    }
    stringstream ss;
    ss << fs.rdbuf();
    fs.close();
    string result = ss.str();
    backtest::data_cache = boost::json::parse(result).as_object();
}

/**
 * Refresh indicator variables for each date and code.
 * @param {string&} date: target date of data_cache. ex) "20230926"
 * @param {string&} code: target stock code to get data in above date.
 * @return -1: Can not find date, try to next date
 * @return -2: Can not find code, try to other code
 * @return 0: success
*/
int backtest::refresh(string& date, string& code) { // TODO: rewrite the return type to int with error code, find date error and find stock error, date error -> break the loop, stock error -> continue the loop
    tm tm;
    strptime(date.c_str(), "%Y%m%d", &tm);
    if(tm.tm_year + 1900 != cur_year) {
        cur_year = tm.tm_year + 1900;
        backtest::utils::load_data("./src/data/datas/krx_" + to_string(tm.tm_year + 1900) + ".txt");
    }
    
    if(backtest::data_cache.find(date) == backtest::data_cache.end())
        return -1;
    boost::json::object day_data = backtest::data_cache[date].as_object();
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
 * Check if tm variable indicate same day.
 * @param {tm&} day1: day 1.
 * @param {tm&} day2: day 2.
*/
bool backtest::utils::tm_is_equal(tm& day1, tm& day2) {
    return day1.tm_year == day2.tm_year
            && day1.tm_mon == day2.tm_mon
            && day1.tm_mday == day2.tm_mday;
}

/**
 * Increment tm type variable for 1 day.
 * @param {tm&} day: target to increment.
*/
void backtest::utils::tm_inc(tm& day) {
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
void backtest::utils::buysell(vector<pair<string, pair<vector<float>, int>>>& buy_list, vector<string>& sell_list) {
    // First, sell if I have the stock in sell list.
    for(auto it : sell_list) {
        auto code = account.find(it);
        if(code != account.end()) {
            cash += (float) (code->second[2] * code->second[0] * (1- backtest::tax - backtest::charge));
            account.erase(code);
            backtest::tr_log.push_back(backtest::date + " SELL - [" + it + "] AVG: " 
                + to_string(code->second[1]) + " CURRENT: " + to_string(code->second[2]) 
                + " * " + to_string(code->second[0]) + " PL rate: " 
                + to_string(100 * (float) code->second[2] / code->second[1] - 100)
                + " Profit Loss: " + to_string((code->second[2] - code->second[1]) * code->second[0]));
        }
    }
    sell_list.clear();    
    
    // Second, buy
    for(auto it : buy_list) {
        int proper_qty = (total_evaluate * max_distribute_rate) / it.second.second; // distribute rate = 20% per stock
        int required_money = it.second.second * (1 + backtest::charge) * proper_qty;
        if(cash < required_money) {
            proper_qty = cash / it.second.second;
            required_money = it.second.second * proper_qty;
            if(proper_qty == 0)
                break;
        }
        auto code = account.find(it.first);
        if(code != account.end()) {
            int before_qty = account[it.first][0];
            
            // Already have stocks
            if(before_qty * account[it.first][1] + required_money > total_evaluate * max_distribute_rate * 2)
                continue;
            
            account[it.first][1] = (account[it.first][1] * before_qty + required_money) / (before_qty + proper_qty);
            account[it.first][0] += proper_qty;
        } else // Add to account
            account[it.first] = {proper_qty, it.second.second, it.second.second};
        
        cash -= (float) (required_money * (1 + backtest::charge));
        backtest::tr_log.push_back(backtest::date + " BUY - [" + it.first + "] PRICE: " + to_string(it.second.second) + " * " + to_string(proper_qty) + " | " + to_string(it.second.first.at(0)) + " | " + to_string(it.second.first.at(1)));
    }
    buy_list.clear();
}

void backtest::utils::refresh_account() {
    total_evaluate = cash;
    for(auto it : account)
        total_evaluate += it.second[0] * it.second[2];
}

void backtest::utils::show_account() {
    cout << "┌----- Account ----------------------------------------------------------------------------------┐" << endl;
    for(auto it : account) {
        cout << "| [" + it.first + "]"
            + " - Qty: " + to_string(it.second[0])
            + "\t Avg price: " + to_string(it.second[1])
            + "\t Cur price: " + to_string(it.second[2])
            + "\t PL Rate: " + to_string((float) 100 * it.second[2] / it.second[1] - 100) + " %" << endl;
    }
    cout << "| Cash: " + to_string(cash) << endl;
    cout << "| Total account evaluation: " + to_string(total_evaluate) << endl;
    cout << "└------------------------------------------------------------------------------------------------┘" << endl;
}

void backtest::utils::clear_account() {
    for(auto it : account) {
        cash += (1 - tax - charge) * it.second[2] * it.second[0];
    }
    account.clear();
}

void backtest::run(string& start, string& end, int cash, vector<string>& target_list) {
    tm tm_today;
    tm tm_end;
    strptime(end.c_str(), "%Y%m%d", &tm_end);
    strptime(start.c_str(), "%Y%m%d", &tm_today);

    char today_format[16];
    vector<pair<string, pair<vector<float>, int>>> buy_list; // {code, {score, price}}, score must be +
    vector<string> sell_list;
    string today;

    backtest::cash = cash;
    backtest::total_evaluate = backtest::cash;

    for(; !backtest::utils::tm_is_equal(tm_today, tm_end); backtest::utils::tm_inc(tm_today)) {
        strftime(today_format, sizeof(today_format), "%Y%m%d", &tm_today);
        today = string(today_format);
        backtest::date = today;
        cout << "Date: " + to_string(tm_today.tm_year + 1900) + "/" + to_string(tm_today.tm_mon + 1) + "/" + to_string(tm_today.tm_mday) << endl;
        if(tm_today.tm_mday == 1) {
            backtest::utils::clear_account();
        }
        if(tm_today.tm_wday == 0 || tm_today.tm_wday == 6) {
            cout << "[ Today is " + string((tm_today.tm_wday == 0) ? "Sunday" : "Saturday") + ". ]" << endl;
            continue;
        }

        // refresh data for each stock and decide to action with score.
        for(int idx = 0; idx < target_list.size(); idx++) {
            string code = target_list[idx];
            int ret = backtest::refresh(today, code);
            if(ret == -1)
                break;
            else if(ret == -2)
                continue;
            
            vector<float> scores = strategy::v0();
            int score = (int) accumulate(scores.begin(), scores.end(), 0);
            if(score > 0)
                buy_list.push_back({code, {scores, indicator::PRICE}});
            
            if(account.find(code) != account.end()) {
                if(score < 0
                    || indicator::PRICE < account[code][1] * (1 - backtest::loss_cut)
                    || indicator::PRICE > account[code][1] * (1 + backtest::profit_cut))
                    sell_list.push_back(code);
                account[code][2] = indicator::PRICE;
            }
        }

        // Sort buy list and transaction
        sort(buy_list.begin(), buy_list.end(), [](pair<string, pair<vector<float>, int>>& a, pair<string, pair<vector<float>, int>>& b){
            return a.second.first >= b.second.first;
        });

        // print account with price
        backtest::utils::refresh_account();
        backtest::utils::buysell(buy_list, sell_list);        
        backtest::utils::show_account();
    }

    cout << "----- [LAST STATUS] -----" << endl;
    for(auto it : tr_log) {
        cout << it << endl;
    }
    backtest::utils::show_account();
}