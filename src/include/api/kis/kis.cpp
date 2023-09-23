#include <iostream>
#include <fstream>

#include <boost/json/serialize.hpp>
#include <boost/json/parse.hpp>

#include "../api.hpp"

using namespace std;

// api::kis namespace -----------------------------------------------------------------------------

string api::kis::htsID;
string api::kis::CANO;
string api::kis::CANO2;
string api::kis::appkey;
string api::kis::appsecret;
string api::kis::token;
string api::kis::approval;

bool api::kis::system = false;
string api::kis::domain = "openapivts.koreainvestment.com";
int api::kis::port = 29443;
string api::kis::ws_domain = "ops.koreainvestment.com";
int api::kis::ws_port = 31000;

boost::json::object api::kis::default_header = {
    {"Content-Type", "application/json; charset=utf-8"},
    {"Host", ""},
};
boost::json::object api::kis::default_tr_header = {
    {"Content-Type", "application/json; charset=utf-8"},
    {"Host", ""},
    {"authorization", ""},
    {"appkey", ""},
    {"appsecret", ""},
    {"tr_id", ""},
    {"tr_cont", ""},
    {"custtype", "P"},
};
boost::json::object api::kis::default_ws_header = {
    {"content-type", "utf-8"},
    {"approval_key", ""},
    {"custtype", "P"},
    {"tr_type", ""},
};

/**
 * Initialize user data.
 * @param {int} argc: argc of main function. number of program arguments.
 * @param {char*[]} argv: argv of main function. string list of program arguments.
 */
void api::kis::init(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "*** Provide user data ***\n<HTS ID>\n<CANO>\n<CANO2>\n<App Key>\n<App Secret>\n");
        exit(1);
    }
    string system_str;
    ifstream fs(argv[1]);
    fs >> api::kis::htsID;
    fs >> api::kis::CANO;
    fs >> api::kis::CANO2;
    fs >> api::kis::appkey;
    fs >> api::kis::appsecret;
    fs >> system_str;
    fs.close();
    if(system_str == "true") {
        api::kis::system = true;
        api::kis::domain = "openapi.koreainvestment.com";
        api::kis::port = 9443;
        api::kis::ws_port = 21000;
    }
    api::kis::default_header["Host"] = api::kis::domain;
    api::kis::default_tr_header["Host"] = api::kis::domain;
    api::kis::default_tr_header["appkey"] = api::kis::appkey;
    api::kis::default_tr_header["appsecret"] = api::kis::appsecret;
}

/**
 * Show user's data.
 * @param {bool} secret: false(default)=don't show secret datas(appkey, appsecret, token, approval). true=show everything.
 */
void api::kis::show_user(bool secret) {
    cout << "┌-------------------------------------------" << endl;
    cout << "| HTS ID: " + api::kis::htsID << endl;
    cout << "| Account Number: " + api::kis::CANO + "-" + api::kis::CANO2 << endl;
    if(secret) {
        cout << "| App Key: " + api::kis::appkey << endl;
        cout << "| App Secret: " + api::kis::appsecret << endl;
        cout << "| Token: " + api::kis::token << endl;
        cout << "| Approval: " + api::kis::approval << endl;
    }
    cout << "└-------------------------------------------" << endl;
}

/**
 * Abstract of net::http::post for KIS Developers API. Return header and body pair.
 * @param {string&} path: path of url. This is inserted to packet header.
 * @param {boost::json::object&} header: header object with json type.
 * @param {boost::json::object&} body: always json type to send.
 */
pair<boost::json::object, boost::json::object> api::kis::post(string& path, boost::json::object& header, boost::json::object& body) {
    string str = net::https::post(api::kis::domain, api::kis::port, path, header, body);
    return net::packet_split(str);
}

/**
 * Abstract of net::http::get for KIS Developers API. Return header and body pair.
 * @param {string&} path: path of url. This is inserted to packet header.
 * @param {boost::json::object&} header: header object with json type.
 * @param {boost::json::object&} body: always json type to send.
 */
pair<boost::json::object, boost::json::object> api::kis::get(string& path, boost::json::object& header, boost::json::object& params) {
    string str = net::https::get(api::kis::domain, api::kis::port, path, header, params);
    return net::packet_split(str);
}

// api::kis websocket send/read -------------------------------------------------------------------

/**
 * Websocket api: send header and body. Transform header and body to appropriate form to fit api::kis
 * @param {net::websocket&} ws_client: web socket client
 * @param {boost::json::object&} header: header
 * @param {boost::json::object&} body: body
*/
pair<boost::json::object, boost::json::object> api::kis::send_ws(net::websocket& ws_client, boost::json::object& header, boost::json::object& body) {
    boost::json::value input = {
        {"input", body},
    };
    boost::json::object packet = {
        {"header", header},
        {"body", input}
    };

    string packet_str = boost::json::serialize(packet);    
    string response_str = ws_client.send(packet_str);
    
    boost::json::object response = boost::json::parse(response_str).as_object();
    return {response["header"].as_object(), response["body"].as_object()};

}
string api::kis::read_ws(net::websocket& ws_client) {
    return ws_client.read();
}

// ------------------------------------------------------------------------------------------------

// api::kis::oauth namespace ----------------------------------------------------------------------

/**
 * Request and store Realtime(Web socket) approval key.
 */
void api::kis::oauth::approval() {
    string path = "/oauth2/Approval";
    boost::json::object header = api::kis::default_header;
    boost::json::object body = {
        {"grant_type", "client_credentials"},
        {"appkey", api::kis::appkey},
        {"secretkey", api::kis::appsecret},
    };
    pair<boost::json::object, boost::json::object> response = api::kis::post(path, header, body);
    api::kis::approval = response.second["approval_key"].as_string();
    api::kis::default_ws_header["approval_key"] = api::kis::approval;
}

/**
 * Request and store access token.
 * @param {bool} revoke: false(default)=request token. true=revoke token.
 */
void api::kis::oauth::token(bool revoke) {
    string path = revoke ? "/oauth2/revokeP" : "/oauth2/tokenP";
    boost::json::object header = api::kis::default_header;
    boost::json::object body = {
        {"appkey", api::kis::appkey},
        {"appsecret", api::kis::appsecret},
    };
    if(revoke)
        body["token"] = api::kis::token;
    else
        body["grant_type"] = "client_credentials";
    pair<boost::json::object, boost::json::object> response = api::kis::post(path, header, body);
    if(revoke) {
        cout << response.second["message"].as_string() << endl;
        api::kis::token = "";
        api::kis::default_tr_header["authorization"] = "";
    } else {
        api::kis::token = response.second["access_token"].as_string();
        api::kis::default_tr_header["authorization"] = "Bearer " + api::kis::token;
    }
}

// ------------------------------------------------------------------------------------------------