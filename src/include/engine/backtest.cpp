#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "engine.hpp"

/**
 * Load data from path as boost::json::object to backtest::data_cache
 * @param {string} path: path of data file.
*/
void backtest::load_data(string path) {
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

int cur_year = 0;
boost::json::object backtest::data_cache;

/**
 * Refresh indicator variables for each date and code.
 * @param {string&} date: target date of data_cache. ex) "20230926"
 * @param {string&} code: target stock code to get data in above date.
*/
bool backtest::refresh(string& date, string& code) {
    tm tm;
    strptime(date.c_str(), "%Y%m%d", &tm);
    if(tm.tm_year + 1900 != cur_year) {
        cur_year = tm.tm_year + 1900;
        load_data("./src/data/datas/kospi_" + to_string(tm.tm_year + 1900) + ".txt");
    }
    
    if(backtest::data_cache.find(date) == backtest::data_cache.end())
        return false;
    boost::json::object day_data = backtest::data_cache[date].as_object();
    if(day_data.find(code) == day_data.end())
        return false;
    
    boost::json::array day_stock = day_data[code].as_array();
    indicator::PRICE = day_stock[0].as_int64();
    indicator::PER = day_stock[2].as_double();
    indicator::PBR = day_stock[4].as_double();
    indicator::AVG_5 = day_stock[5].as_double();
    indicator::AVG_20 = day_stock[6].as_double();
    indicator::AVG_60 = day_stock[7].as_double();
    indicator::RSI_9 = day_stock[8].as_double();
    indicator::RSI_14 = day_stock[9].as_double();
    indicator::RSI_28 = day_stock[10].as_double();
    indicator::BOLLINGER_LOW = day_stock[11].as_double();
    indicator::BOLLINGER_HIGH = day_stock[12].as_double();

    return true;
}

/**
 * Check if tm variable indicate same day.
 * @param {tm&} day1: day 1.
 * @param {tm&} day2: day 2.
*/
bool tm_is_equal(tm& day1, tm& day2) {
    return day1.tm_year == day2.tm_year
            && day1.tm_mon == day2.tm_mon
            && day1.tm_mday == day2.tm_mday;
}

/**
 * Increment tm type variable for 1 day.
 * @param {tm&} day: target to increment.
*/
void tm_inc(tm& day) {
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

vector<string> backtest::tr_log;

/**
 * Backtesting only. Buy or sell stocks by specified conditions.
 * @param {map<string, array<int, 3>>&} account: current stock account
 * @param {int&} total_evaluate: current total evaluation
 * @param {int&} cash: current cash
 * @param {vector<pair<string, pair<int, int>>>&} buy_list: buy targets
 * @param {vector<pair<string, pair<int, int>>>&} sell_list: sell targets
*/
void buysell(map<string, array<int, 3>>& account, int& total_evaluate, int& cash, vector<pair<string, pair<int, int>>>& buy_list, vector<pair<string, pair<int, int>>>& sell_list) {
    float tax = 0.002;
    float charge = 0.00015;
    float max_distribute_rate = 0.2;

    // Exceed the standard loss rate
    vector<string> rate_sell_list;
    for(auto it : account) {
        float profit_loss = 100 * it.second[2] / it.second[1] - 100;
        if(profit_loss < -2) {
            cash += (float) (it.second[2] * it.second[0] * (1-tax-charge));
            backtest::tr_log.push_back("SELL: [" + it.first + "]: " + to_string(it.second[1]) + " -> " + to_string(it.second[2]) + " x " + to_string(it.second[0]) + " => " + to_string(total_evaluate));
            rate_sell_list.push_back(it.first);
        }
    }
    for(auto it : rate_sell_list) {
        account.erase(it);
    }

    // First, sell if I have the stock in sell list.
    for(auto it : sell_list) {
        auto code = account.find(it.first);
        if(code != account.end()) {
            cash += (float) (code->second[2] * code->second[0] * (1-tax-charge));
            account.erase(code);
            backtest::tr_log.push_back("SELL: [" + it.first + "]: " + to_string(code->second[1]) + " -> " + to_string(it.second.second) + " x " + to_string(code->second[0]) + " => " + to_string(total_evaluate));
        }
    }
    sell_list.clear();
    // Buy
    for(auto it : buy_list) {
        int proper_qty = (total_evaluate * max_distribute_rate) / it.second.second; // distribute rate = 20% per stock
        int required_money = it.second.second * proper_qty;
        if(cash < required_money) {
            proper_qty = cash / it.second.second;
            required_money = it.second.second * proper_qty;
            if(proper_qty == 0)
                break;
        }
        auto code = account.find(it.first);
        if(code != account.end()) {
            int before_qty = account[it.first][0];
            
            account[it.first][1] = (account[it.first][1] * before_qty + required_money) / (before_qty + proper_qty);
            account[it.first][0] += proper_qty;
        } else {
            // Add to account
            account[it.first] = {proper_qty, it.second.second, it.second.second};
        }
        cash -= (float) (required_money * (1+charge));
        backtest::tr_log.push_back("BUY: [" + it.first + "]: " + to_string(it.second.second) + " x " + to_string(proper_qty) + " => " + to_string(total_evaluate));
    }
    buy_list.clear();
}

void refresh_account(map<string, array<int, 3>>& account, int& total_evaluate, int& cash) {
    total_evaluate = cash;
    for(auto it : account) {
        total_evaluate += it.second[0] * it.second[2];
    }
}

void show_account(map<string, array<int, 3>>& account, int& total_evaluate, int& cash) {
    cout << "┌----- Account --------------------------------------------------------------------┐" << endl;
    for(auto it : account) {
        cout << "| [" + it.first + "]"
            + " - Qty: " + to_string(it.second[0])
            + ", Avg price: " + to_string(it.second[1])
            + ", Cur price: " + to_string(it.second[2])
            + ", PL Rate: " + to_string((float) 100 * it.second[2] / it.second[1] - 100) + " %" << endl;
    }
    cout << "| Cash: " + to_string(cash) << endl;
    cout << "| Total account evaluation: " + to_string(total_evaluate) << endl;
    cout << "└----------------------------------------------------------------------------------┘" << endl;
}

void backtest::run(string& start, string& end, int cash, vector<string>& target_list) {
    tm tm_today;
    tm tm_end;
    strptime(end.c_str(), "%Y%m%d", &tm_end);
    strptime(start.c_str(), "%Y%m%d", &tm_today);

    char today_format[32];
    string today;
    map<string, array<int, 3>> account; // {code, {qty, avg_price, current_price}}
    vector<pair<string, pair<int, int>>> buy_list; // {code, {score, price}}, score must be +
    vector<pair<string, pair<int, int>>> sell_list; // {code, {score, price}}, score must be -

    int total_evaluate = cash;

    for( ; !tm_is_equal(tm_today, tm_end) ; tm_inc(tm_today)) {
        strftime(today_format, sizeof(today_format), "%Y%m%d", &tm_today);
        today = string(today_format);
        cout << "Date: " + to_string(tm_today.tm_year + 1900) + "/" + to_string(tm_today.tm_mon + 1) + "/" + to_string(tm_today.tm_mday) << endl;
        if(tm_today.tm_wday == 0 || tm_today.tm_wday == 6) {
            cout << "[ Today is " + string((tm_today.tm_wday == 0) ? "Sunday" : "Saturday") + ". ]" << endl;
            continue;
        }

        // refresh data for each stock and decide to action with score.
        for(auto it : target_list) {
            string code = it;
            if(!backtest::refresh(today, code))
                break;
            int score = strategy::v0();
            if(score > 0)
                buy_list.push_back({code, {score, indicator::PRICE}});
            else if(score < 0)
                sell_list.push_back({code, {score, indicator::PRICE}});
            
            if(account.find(code) != account.end()) {
                account[code][2] = indicator::PRICE;
            }
        }

        // Sort buy list and transaction
        sort(buy_list.begin(), buy_list.end(), [](pair<string, pair<int, int>>& a, pair<string, pair<int, int>>& b){
            return a.second.first >= b.second.first;
        });
        // print account with price
        refresh_account(account, total_evaluate, cash);
        buysell(account, total_evaluate, cash, buy_list, sell_list);
        
        refresh_account(account, total_evaluate, cash);
        show_account(account, total_evaluate, cash);
    }

    cout << "\n\n----- [LAST STATUS] -----" << endl;
    for(auto it : backtest::tr_log) {
        cout << it << endl;
    }
    refresh_account(account, total_evaluate, cash);
    show_account(account, total_evaluate, cash);
}