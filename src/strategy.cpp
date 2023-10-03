#include "include/runtime/runtime.hpp"

vector<string> same_sector_code(string& code);
pair<double, double> sector_avg_per_pbr(vector<string>& same_sector_list);

float runtime::strategy(string& code) {
    float score = 0;

    double per = indicator::PER->operator()(cache[code].last_idx);
    double pbr = indicator::PBR->operator()(cache[code].last_idx);
    vector<string> same_sector_list = same_sector_code(code);    
    pair<double, double> avg_per_pbr = sector_avg_per_pbr(same_sector_list);
    
    if(per < 12 && per < avg_per_pbr.first * 0.8)
        score += 5 * per;
    else if(per > 30 || per > avg_per_pbr.first * 1.5)
        score -= per;
    
    if(pbr < 1.2 && pbr < avg_per_pbr.second * 0.8)
        score += 10 * pbr;
    else if(pbr > avg_per_pbr.second * 1.5)
        score -= 5 * pbr;

    int idx = cache[code].last_idx;
    string date = cache[code].last_date;

    vector<pair<int, bool>> cross_AVG_20_60 = series::crossing(indicator::AVG_20, indicator::AVG_60, idx);
    if(!cross_AVG_20_60.empty()) {
        if(cross_AVG_20_60.back().second && cross_AVG_20_60.back().first + 1 > idx)
            score += 20;
        else if(!cross_AVG_20_60.back().second && cross_AVG_20_60.back().first + 1 > idx)
            score -= 20;
    }

    vector<pair<int, bool>> price_cross_lbb = series::crossing(*indicator::CLOSE, indicator::BOLLINGER_LOW, idx);
    if(!price_cross_lbb.empty()) {
        // Last Bollinger Band Minimum crossing is below to above occured in last 5 days.
        if(price_cross_lbb.back().second == true
            && price_cross_lbb.back().first + 5 > idx) {
            score += 30;
        }
    }
    vector<pair<int, bool>> price_cross_hbb = series::crossing(*indicator::CLOSE, indicator::BOLLINGER_HIGH, idx);
    if(!price_cross_hbb.empty()) {
        // Last Bollinger Band Maximum crossing is above to below occured in last 5 days.
        if(price_cross_hbb.back().second == false
            && price_cross_hbb.back().first + 5 > idx) {
            score -= 30;
        }
    }


    return score;
}

vector<string> same_sector_code(string& code) {
    string sector_code = "";
    string market = "";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(runtime::db, ("SELECT sector, market FROM STOCK_INFO WHERE code=" + code).c_str(), -1, &stmt, NULL);
    if(sqlite3_step(stmt) == SQLITE_ROW) {
        sector_code = (const char*) sqlite3_column_text(stmt, 0);
        market = (const char*) sqlite3_column_text(stmt, 1);
    }
    sqlite3_finalize(stmt);
    
    vector<string> same_sector_list;
    sqlite3_prepare_v2(runtime::db, ("SELECT code FROM STOCK_INFO WHERE market='" + market + "' AND sector=" + sector_code).c_str(), -1, &stmt, NULL);
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        same_sector_list.push_back((const char*) sqlite3_column_text(stmt, 0));
    }
    sqlite3_finalize(stmt);
    return same_sector_list;
}

pair<double, double> sector_avg_per_pbr(vector<string>& same_sector_list) {
    double avg_of_sector_per = 0;
    double avg_of_sector_pbr = 0;
    int i;
    for(i = 0; i < same_sector_list.size(); i++) {
        string same_sector = same_sector_list[i];
        int idx = runtime::cache[same_sector].last_idx;
        avg_of_sector_per += runtime::cache[same_sector].data[6](idx);
        avg_of_sector_pbr += runtime::cache[same_sector].data[7](idx);
    }
    avg_of_sector_per /= i;
    avg_of_sector_pbr /= i;
    return {avg_of_sector_per, avg_of_sector_pbr};
}