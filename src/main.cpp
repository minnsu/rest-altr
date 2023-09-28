#include <iostream>
#include <fstream>
#include <vector>

#include "include/net/net.hpp"
#include "include/api/api.hpp"
#include "include/engine/engine.hpp"

int main(int argc, char *argv[]) {
    // api::kis::init(argc, argv);
    // api::kis::oauth::approval();
    // api::kis::oauth::token();
    // net::websocket ws_client(api::kis::ws_domain, api::kis::ws_port);

    // api::kis::show_user(true);

    std::string start = "20230801";
    std::string end = "20230925";

    string code;
    std::vector<std::string> target_list;
    std::ifstream kospi_file("./src/data/codes/KOSPI.txt");
    while(getline(kospi_file, code)) {
        target_list.push_back(code);
    }
    kospi_file.close();
    std::ifstream kosdaq_file("./src/data/codes/KOSDAQ.txt");
    while(getline(kosdaq_file, code)) {
        target_list.push_back(code);
    }
    kosdaq_file.close();

    backtest::run(start, end, 10000000, target_list);

    return 0;
}