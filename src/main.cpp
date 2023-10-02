#include <iostream>
#include <fstream>
#include <vector>

#include <sqlite3.h>

#include "include/net/net.hpp"
#include "include/api/api.hpp"

#include "include/runtime/backtest/backtest.hpp"
#include "include/runtime/engine/engine.hpp"

void program_init(int argc, char *argv[]) {
    api::kis::init(argc, argv);
    api::kis::oauth::approval();
    api::kis::oauth::token();
    net::websocket ws_client(api::kis::ws_domain, api::kis::ws_port);

    api::kis::show_user(true);
}

void backtesting(string start, string end, int init_cash);

int main(int argc, char *argv[]) {
    // program_init(argc, argv);

    backtesting("20230701", "20230927", 10000000);
    return 0;
}

void backtesting(string start, string end, int init_cash) {
    string code;
    std::vector<std::string> target_list;

    sqlite3 *db;
    sqlite3_stmt *stmt;
    sqlite3_open("./db/stocks.db", &db);
    
    string query = "SELECT code FROM STOCK_INFO WHERE market='KOSPI';";
    int rc = sqlite3_prepare_v2(db, query.c_str(),-1, &stmt, NULL);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL Error: \n");
        exit(1);
    }
    
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        target_list.push_back((const char *) sqlite3_column_text(stmt, 0));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    backtest::run(start, end, 10000000, target_list);
}