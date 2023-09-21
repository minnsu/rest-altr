#include <iostream>

#include <boost/json/serialize.hpp>

#include "include/net.hpp"
#include "include/api.hpp"

int main(int argc, char *argv[]) {
    api::kis::init(argc, argv);
    api::kis::oauth::approval();
    api::kis::oauth::token();

    api::kis::show_user(true);

    std::string code = "005930";
    bool attach = true;
    

    net::websocket ws_client(api::kis::ws_domain, api::kis::ws_port);
    // ws ccnl price attach test.
    std::string path = "/tryitout/H0STCNT0";

    boost::json::object header = api::kis::default_ws_header;
    header["tr_type"] = attach ? "1" : "2";
    boost::json::object body = {
        {"tr_id", "H0STCNT0"},
        {"tr_key", code},
    };
    std::cout << ws_client.send(path, header, body) << std::endl;
    // while(true) {
    //     std::cout << ws_client.read() << std::endl;
    // }

    // std::pair<boost::json::object, boost::json::object> response;
    // response = api::kis::domestic::conclude_price(attach, code);
    // std::cout << boost::json::serialize(response.first) << std::endl;
    // std::cout << boost::json::serialize(response.second) << std::endl;
    
    return 0;
}