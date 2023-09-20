#include <iostream>

#include <boost/json/serialize.hpp>

#include "include/api.hpp"

int main(int argc, char *argv[]) {
    api::kis::init(argc, argv);
    api::kis::oauth::approval();
    api::kis::oauth::token();

    api::kis::show_user();
    
    std::string code = "005930";
    std::string start = "20220104";
    std::string end = "20230406";
    std::string time = "121514";
    std::string type = "300";
    std::string sort = "1";
    std::pair<boost::json::object, boost::json::object> response;

    response = api::kis::domestic::sell(code, 2, 55000);
    std::cout << boost::json::serialize(response.first) << std::endl;
    std::cout << boost::json::serialize(response.second) << std::endl;
    
    return 0;
}