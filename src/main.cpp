#include <iostream>
#include <fstream>

#include <boost/json/serialize.hpp>

#include "include/mynet.hpp"

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "*** Provide user data ***\n<HTS ID>\n<CANO>\n<CANO2>\n<App Key>\n<App Secret>\n");
        exit(1);
    }
    std::ifstream fs;
    fs.open(argv[1]);
    std::string htsID;
    fs >> htsID;
    std::string CANO;
    fs >> CANO;
    std::string CANO2;
    fs >> CANO2;
    std::string appkey;
    fs >> appkey;
    std::string appsecret;
    fs >> appsecret;
    fs.close();

    int port = 29443;
    mynet::http client;
    std::string domain = "openapivts.koreainvestment.com";
    std::string path = "/oauth2/tokenP";
    std::string url = "https://" + domain + ":" + std::to_string(port) + path;

    
    boost::json::object header = mynet::default_header;
    header["Host"] = "openapivts.koreainvestment.com";
    
    boost::json::object body;
    body["grant_type"] = "client_credentials";
    body["appkey"] = appkey;
    body["appsecret"] = appsecret;

    std::string response = client.post(url, header, body);

    std::cout << "*** RESPONSE PACKET ***\n" + response + "\n\n" << std::endl;
    std::pair<boost::json::object, boost::json::object> packet_pair = mynet::packet_split(response);

    return 0;
}