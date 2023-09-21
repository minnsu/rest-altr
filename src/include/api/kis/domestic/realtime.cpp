#include "../../api.hpp"

using namespace std;

// api::kis::domestic namespace, Realtime functions ------------------------------------------------

/**
 * Domestic: Realtime conclude price attach/detach.
 * @param {bool} attach: true=attach, false=detach
 * @param {string&} code: stock code
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::conclude_price(net::websocket& ws_client, bool attach, string& code) {
    string path = "/tryitout/H0STCNT0";

    boost::json::object header = api::kis::default_ws_header;
    header["tr_type"] = attach ? "1" : "2";
    boost::json::object body = {
        {"tr_id", "H0STCNT0"},
        {"tr_key", code},
    };
    return api::kis::send_ws(ws_client, header, body);
}

/**
 * Domestic: Realtime asking price attach/detach.
 * @param {bool} attach: true=attach, false=detach
 * @param {string&} code: stock code
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::asking_price(net::websocket& ws_client, bool attach, string& code) {
    string path = "/tryitout/H0STASP0";

    boost::json::object header = api::kis::default_ws_header;
    header["tr_type"] = attach ? "1" : "2";
    boost::json::object body = {
        {"tr_id", "H0STASP0"},
        {"tr_key", code},
    };
    return api::kis::send_ws(ws_client, header, body);
}

/**
 * Domestic: Realtime conclude notify
 * @param {bool} attach: true=attach, false=detach
 * @param {string&} code: stock code
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::conclude_notify(net::websocket& ws_client, bool attach) {
    string path = "/tryitout/H0STCNI0";

    boost::json::object header = api::kis::default_ws_header;
    header["tr_type"] = attach ? "1" : "2";
    boost::json::object body = {
        {"tr_id", (api::kis::system ? "H0STCNI0" : "H0STCNI9")},
        {"tr_key", api::kis::htsID},
    };
    return api::kis::send_ws(ws_client, header, body);
}

// ------------------------------------------------------------------------------------------------