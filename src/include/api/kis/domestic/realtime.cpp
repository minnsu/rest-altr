#include "../../api.hpp"

using namespace api::kis;

// api::kis::domestic namespace, Realtime functions ------------------------------------------------

/**
 * Domestic: Realtime conclude price attach/detach.
 * @param {bool} attach: true=attach, false=detach
 * @param {string&} code: stock code
*/
pair<json::object, json::object> domestic::conclude_price(net::websocket& ws_client, bool attach, string& code) {
    json::object header = api::kis::default_ws_header;
    header["tr_type"] = attach ? "1" : "2";
    json::object body = {
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
pair<json::object, json::object> domestic::asking_price(net::websocket& ws_client, bool attach, string& code) {
    json::object header = api::kis::default_ws_header;
    header["tr_type"] = attach ? "1" : "2";
    json::object body = {
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
pair<json::object, json::object> domestic::conclude_notify(net::websocket& ws_client, bool attach) {
    json::object header = api::kis::default_ws_header;
    header["tr_type"] = attach ? "1" : "2";
    json::object body = {
        {"tr_id", (api::kis::system ? "H0STCNI0" : "H0STCNI9")},
        {"tr_key", api::kis::htsID},
    };
    return api::kis::send_ws(ws_client, header, body);
}

// ------------------------------------------------------------------------------------------------