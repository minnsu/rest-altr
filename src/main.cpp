#include <iostream>

#include "include/net/net.hpp"
#include "include/api/api.hpp"
#include "include/engine/engine.hpp"

int main(int argc, char *argv[]) {
    // api::kis::init(argc, argv);
    // api::kis::oauth::approval();
    // api::kis::oauth::token();
    // net::websocket ws_client(api::kis::ws_domain, api::kis::ws_port);

    // api::kis::show_user(true);

    std::string start = "20221220";
    std::string end = "20230120";
    backtest::run(start, end, 10000000);

    return 0;
}