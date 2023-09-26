#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "engine.hpp"

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
    backtest::year_data = boost::json::parse(result).as_object();
}

int cur_year = 0;
boost::json::object backtest::year_data;

bool backtest::refresh(string& date, string& code) {
    tm tm;
    strptime(date.c_str(), "%Y%m%d", &tm);
    if(tm.tm_year + 1900 != cur_year) {
        cur_year = tm.tm_year + 1900;
        load_data("./src/data/datas/kospi_" + to_string(tm.tm_year + 1900) + ".txt");
    }
    
    if(backtest::year_data.find(date) == backtest::year_data.end())
        return false;
    boost::json::object day_data = backtest::year_data[date].as_object();
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

bool tm_is_equal(tm& day1, tm& day2) {
    return day1.tm_year == day2.tm_year
            && day1.tm_mon == day2.tm_mon
            && day1.tm_mday == day2.tm_mday;
}

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

void backtest::run(string& start, string& end, int cash, vector<string>& target_list) {
    tm tm_today;
    tm tm_end;
    strptime(end.c_str(), "%Y%m%d", &tm_end);
    strptime(start.c_str(), "%Y%m%d", &tm_today);

    char print_day[128];
    map<string, array<int, 3>> account; // {code, {qty, avg_price, current_price}}
    vector<pair<string, pair<int, int>>> buy_list; // {code, {score, price}}, score must be +
    vector<pair<string, pair<int, int>>> sell_list; // {code, {score, price}}, score must be -

    int total_evaluate = cash;
    float charge = 0.015;
    float tax = 0.2;

    for( ; !tm_is_equal(tm_today, tm_end) ; tm_inc(tm_today)) {
        strftime(print_day, sizeof(print_day), "%Y%m%d", &tm_today);
        string today = string(print_day);
        cout << "\nDate: " + today + "\n" << endl;
        if(tm_today.tm_wday == 0) {
            cout << "[ Today is Sunday. ]" << endl;
            continue;
        } else if(tm_today.tm_wday == 6) {
            cout << "[ Today is Saturday. ]" << endl;
            continue;
        }

        for(auto it : target_list) {
            string code = it;
            if(!backtest::refresh(today, code))
                break;
            int score = strategy::v0();
            if(score > 0)
                buy_list.push_back({code, {score, indicator::PRICE}});
            else if(score < 0)
                sell_list.push_back({code, {score, indicator::PRICE}});
            
            auto have_stock = account.find(code);
            if(have_stock != account.end()) {
                account[code][2] = indicator::PRICE;
            }
        }

        // Sort buy list and Buy
        sort(buy_list.begin(), buy_list.end(), [](pair<string, pair<int, int>>& a, pair<string, pair<int, int>>& b){
            return a.second.first >= b.second.first;
        });
        for(auto it : buy_list) {
            int proper_qty = (total_evaluate / 5) / it.second.second; // distribute rate = 20% per stock
            int require_money = it.second.second * proper_qty;
            if(cash < require_money) {
                proper_qty = cash / it.second.second;
                require_money = it.second.second * proper_qty;
                if(proper_qty == 0)
                    break;
            }
            cash -= require_money * (1+charge);
            cout << "BUY: [" + it.first + "]: " + to_string(it.second.second) + " x " + to_string(proper_qty) << endl;
            auto code = account.find(it.first);
            if(code != account.end()) {
                // already have that stocks. just add to
                int before_qty = account[it.first][0];
                account[it.first][1] = (account[it.first][1] * before_qty + require_money) / (before_qty + proper_qty);
                account[it.first][0] += proper_qty;
            } else {
                // Add to account
                account[it.first][0] = proper_qty;
                account[it.first][1] = it.second.second;
                account[it.first][2] = it.second.second;
            }
        }
        // Sell if I have the stock in sell list.
        for(auto it : sell_list) {
            auto code = account.find(it.first);
            if(code != account.end()) {
                cout << "SELL: [" + it.first + "]: " + to_string(it.second.second) + " x " + to_string(code->second[0]) << endl;
                cash += (float) (code->second[2] * code->second[0] * (1-tax-charge));
                account.erase(code);
            }
        }

        // print account with price
        total_evaluate = cash;
        cout << "┌----- Account --------------------------------------------------------------------┐" << endl;
        for(auto it : account) {
            cout << "| [" + it.first + "]"
                + " - Qty: " + to_string(it.second[0])
                + ", Avg price: " + to_string(it.second[1])
                + ", Cur price: " + to_string(it.second[2])
                + ", PL Rate: " + to_string((float) 100 * it.second[2] / it.second[1] - 100) + " %" << endl;
            total_evaluate += it.second[0] * it.second[2];
        }
        cout << "| Cash: " + to_string(cash) << endl;
        cout << "| Total account evaluation: " + to_string(total_evaluate) << endl;
        cout << "└----------------------------------------------------------------------------------┘" << endl;

        buy_list.clear();
        sell_list.clear();
        // sleep(1);
    }

}