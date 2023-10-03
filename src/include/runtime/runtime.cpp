#include <cstdio>

#include "runtime.hpp"

using namespace std;

Series* runtime::indicator::OPEN = nullptr;
Series* runtime::indicator::HIGH = nullptr;
Series* runtime::indicator::LOW = nullptr;
Series* runtime::indicator::CLOSE = nullptr;
Series* runtime::indicator::VOLUME = nullptr;
Series* runtime::indicator::CHANGE = nullptr;
Series* runtime::indicator::PER = nullptr;
Series* runtime::indicator::PBR = nullptr;
Series runtime::indicator::AVG_5 = RowVectorXd::Zero(270);
Series runtime::indicator::AVG_20 = RowVectorXd::Zero(270);
Series runtime::indicator::AVG_60 = RowVectorXd::Zero(270);
Series runtime::indicator::RSI_9 = RowVectorXd::Zero(270);
Series runtime::indicator::RSI_14 = RowVectorXd::Zero(270);
Series runtime::indicator::RSI_28 = RowVectorXd::Zero(270);
Series runtime::indicator::BOLLINGER_LOW = RowVectorXd::Zero(270);
Series runtime::indicator::BOLLINGER_HIGH = RowVectorXd::Zero(270);

const float runtime::param::TAX = 0.002;
const float runtime::param::CHARGE = 0.00015;

const float runtime::param::MAX_DIST_RATE = 0.2;
const float runtime::param::PROFIT_CUT = 0.2;
const float runtime::param::LOSS_CUT = 0.05;

float runtime::strategy::v0(string& code) {
    float score = 0;
    
    double per = indicator::PER->operator()(cache[code].last_idx);
    double pbr = indicator::PBR->operator()(cache[code].last_idx);

    if(per < 10)
        score += 3 * per;
    else if(per > 30)
        score -= per / 5;
    
    if(pbr < 1.2)
        score += 10 * pbr;
    else if(pbr > 3)
        score -= 4 * pbr;

    return score;
}

sqlite3 *runtime::db;
map<string, runtime::_CACHE> runtime::cache;


/**
 * 
*/
void runtime::DB::SQL_OPEN() {
    printf("Database opening.. ");
    
    int rc = sqlite3_open("./db/stocks.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can not open database: \n%s", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_exec(db, "PRAGMA cache_size=16384;", NULL, NULL, NULL);
    sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
    
    printf("Done.\n");
}

/**
 * 
*/
void runtime::DB::SQL_CACHING(string& start_date, string& end_date, vector<string>& target_list) {
    printf("Database caching.. \n");
    
    tm tm_start, tm_end;
    char one_year_past[10];
    strptime(start_date.c_str(), "%Y%m%d", &tm_start);
    strptime(end_date.c_str(), "%Y%m%d", &tm_end);

    tm_start.tm_year -= 1;
    strftime(one_year_past, sizeof(one_year_past), "%Y%m%d", &tm_start);
    int days = (tm_end.tm_year - tm_start.tm_year) * 280 
                + (tm_end.tm_mon - tm_start.tm_mon) * 24
                + ((tm_end.tm_mday - tm_start.tm_mday) * 5 / 7);  // use to make size Series

    sqlite3_stmt *stmt;
    string market;
    
    int rc;
    for(auto& code : target_list) {
        string query = "SELECT market FROM STOCK_INFO WHERE code='" + code + "';";
        rc = sqlite3_prepare_v2(runtime::db, query.c_str(), -1, &stmt, NULL);
        
        if(sqlite3_step(stmt) == SQLITE_ROW) {
            market = (const char*) sqlite3_column_text(stmt, 0);
        } else {
            printf("Not found stock code : %s\n", code.c_str());
            sqlite3_finalize(stmt);
            continue;
        }
        sqlite3_finalize(stmt);

        // caching before one year ago from start_date to start_date
        query = "SELECT * FROM " + market + " WHERE code='" + code + "' AND " + "date >= '" + one_year_past + "' AND date < '" + start_date + "';";
        rc = sqlite3_prepare_v2(runtime::db, query.c_str(), -1, &stmt, NULL);

        array<Series, 8> cache_stock = {
            Series::Zero(days), Series::Zero(days), Series::Zero(days), Series::Zero(days),
            Series::Zero(days), Series::Zero(days), Series::Zero(days), Series::Zero(days)
        };

        int idx = 0;
        for(idx = 0; sqlite3_step(stmt) == SQLITE_ROW; idx++) {
            cache_stock[0](idx) = sqlite3_column_double(stmt, 2);
            cache_stock[1](idx) = sqlite3_column_double(stmt, 3);
            cache_stock[2](idx) = sqlite3_column_double(stmt, 4);
            cache_stock[3](idx) = sqlite3_column_double(stmt, 5);
            cache_stock[4](idx) = sqlite3_column_double(stmt, 6);
            cache_stock[5](idx) = sqlite3_column_double(stmt, 7);
            cache_stock[6](idx) = sqlite3_column_double(stmt, 8);
            cache_stock[7](idx) = sqlite3_column_double(stmt, 9);
        }
        sqlite3_finalize(stmt);
        
        cache[code] = runtime::_CACHE {
            start_date, idx, cache_stock
        };
        printf(" -> : %s (%ld%%) \r", code.c_str(), 100 * cache.size() / target_list.size());
    }
    printf("\nDone.\n");
}

/**
 * 
*/
void runtime::DB::SQL_ERROR_CHECK(int rc, char *ErrMsg) {
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", ErrMsg);
        sqlite3_free(ErrMsg);
        sqlite3_close(db);
        exit(1);
    }
}