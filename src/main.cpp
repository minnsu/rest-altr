#include <iostream>

#include <boost/json/serialize.hpp>

#include "include/api.hpp"

int main(int argc, char *argv[]) {
    api::kis::init(argc, argv);
    api::kis::oauth::approval();
    api::kis::oauth::token();
    std::cout << "HELLO" << std::endl;

    api::kis::show_user();
    std::pair<boost::json::object, boost::json::object> response;

    response = api::kis::domestic::account_balance();
    std::cout << boost::json::serialize(response.first) << std::endl;
    std::cout << boost::json::serialize(response.second) << std::endl;
    
    return 0;
}