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
boost::json::array backtest::get_data(string& date, string& code) {
    tm tm;
    strptime(date.c_str(), "%Y%m%d", &tm);
    if(tm.tm_year + 1900 != cur_year) {
        cur_year = tm.tm_year + 1900;
        load_data("./src/data/datas/kospi_" + to_string(tm.tm_year + 1900) + ".txt");
    }
    if(backtest::year_data.find(date) == backtest::year_data.end()) {
        return NULL;
    }
    boost::json::object day_data = backtest::year_data[date].as_object();
    return day_data[code].as_array();
}


float backtest::RSI(string& code) {
    return 0.1;
}
float backtest::PER(string& code) {
    return 0.1;
}
float backtest::PBR(string& code) {
    return 0.1;
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

void backtest::run(string& start, string& end, int cash) {
    tm tm_today;
    tm tm_end;
    strptime(end.c_str(), "%Y%m%d", &tm_end);
    strptime(start.c_str(), "%Y%m%d", &tm_today);

    char print_day[128];
    string today, code;
    boost::json::array data;
    while(!tm_is_equal(tm_today, tm_end)) {

        
        strftime(print_day, sizeof(print_day), "%Y%m%d", &tm_today);
        today = string(print_day);
        cout << today << endl;

        code = "005930";
        data = backtest::get_data(today, code);
        cout << boost::json::serialize(data) << endl;

        tm_inc(tm_today);
    }

}