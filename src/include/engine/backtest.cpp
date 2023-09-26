#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>

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
    string today;
    while(!tm_is_equal(tm_today, tm_end)) {
        strftime(print_day, sizeof(print_day), "%Y%m%d", &tm_today);
        today = string(print_day);
        cout << "Date: " + today << endl;

        for(auto it : target_list) {
            string code = it;
            if(!backtest::refresh(today, code))
                break;
            cout << "Stock code: " + code << endl;
            int action = strategy::v0();
            switch(action) {
            case 0: // hold
                break;
            case 1: // buy
                break;
            case 2: // sell
                break;
            }
        }

        tm_inc(tm_today);
    }

}