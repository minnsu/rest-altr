#include <iostream>
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

    std::string start = "20230901";
    std::string end = "20230925";

    std::vector<std::string> target_list = {"005930", "000660", "005380"};
    backtest::run(start, end, 10000000, target_list);

    return 0;
}