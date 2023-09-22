#include <iostream>

#include <boost/json/serialize.hpp>
#include <boost/json/parse.hpp>

#include "include/net/net.hpp"
#include "include/api/api.hpp"

int main(int argc, char *argv[]) {
    api::kis::init(argc, argv);
    api::kis::oauth::approval();
    api::kis::oauth::token();
    net::websocket ws_client(api::kis::ws_domain, api::kis::ws_port);

    api::kis::show_user(true);

    std::string code = "005930";
    bool attach = true;
    
    std::pair<boost::json::object, boost::json::object> response = api::kis::domestic::conclude_notify(ws_client, attach);
    std::cout << boost::json::serialize(response.first) << std::endl;
    std::cout << boost::json::serialize(response.second) << std::endl;
    response = api::kis::domestic::conclude_price(ws_client, attach, code);
    std::cout << boost::json::serialize(response.first) << std::endl;
    std::cout << boost::json::serialize(response.second) << std::endl;
    
    while(true) {
        std::string response = api::kis::read_ws(ws_client);
        if(response[0] == '{') {
            boost::json::object json = boost::json::parse(response).as_object();
            if(json["header"].as_object()["tr_id"] == "PINGPONG")
                ws_client.pong(response);
        }
        std::cout << response << std::endl;
    }

    return 0;
}