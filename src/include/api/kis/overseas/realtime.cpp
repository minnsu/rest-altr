#include "../../api.hpp"

using namespace api::kis;

// api::kis::overseas namespace, Realtime functions ------------------------------------------------

/**
 * Overseas: Realtime conclude price attach/detach.
 * @param {bool} attach: true=attach, false=detach
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).
 * @param {string&} code: stock code
 * @param {bool} after_close: false=default. true=for us market, overtime concluded price inquire(10AM ~ 4PM in korea).
*/
pair<json::object, json::object> overseas::conclude_price(net::websocket& ws_client, bool attach, overseas::market market, string& code, bool after_close) {
    string m_code = after_close ? ("R" + overseas::exchan_code[11 + market]) : ("D" + overseas::exchan_code[market]);
    
    json::object header = api::kis::default_ws_header;
    header["tr_type"] = attach ? "1" : "2";
    json::object body = {
        {"tr_id", "HDFSCNT0"},
        {"tr_key", m_code + code},
    };
    return api::kis::send_ws(ws_client, header, body);
}

/**
 * Overseas: Realtime asking price in asia markets attach/detach. Currently delayed asking price only supported.
 * @param {bool} attach: true=attach, false=detach
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).
 * @param {string&} code: stock code
*/
pair<json::object, json::object> overseas::asking_price_asia(net::websocket& ws_client, bool attach, overseas::market market, string& code) {
    string m_code = "D" + overseas::exchan_code[market];
    
    json::object header = api::kis::default_ws_header;
    header["tr_type"] = attach ? "1" : "2";
    json::object body = {
        {"tr_id", "HDFSASP1"},
        {"tr_key", m_code + code},
    };
    return api::kis::send_ws(ws_client, header, body);
}

/**
 * Overseas: Realtime conclude notify.
 * @param {bool} attach: true=attach, false=detach
*/
pair<json::object, json::object> overseas::conclude_notify(net::websocket& ws_client, bool attach) {
    json::object header = api::kis::default_ws_header;
    header["tr_type"] = attach ? "1" : "2";
    json::object body = {
        {"tr_id", "H0GSCNI0"},
        {"tr_key", api::kis::htsID},
    };
    return api::kis::send_ws(ws_client, header, body);
}

/**
 * Overseas: Realtime asking price in us markets attach/detach. Currently first asking price only supported.
 * @param {bool} attach: true=attach, false=detach
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).
 * @param {string&} code: stock code
*/
pair<json::object, json::object> overseas::asking_price_us(net::websocket& ws_client, bool attach, overseas::market market, string& code, bool after_close) {
    string m_code = after_close ? ("R" + overseas::exchan_code[11 + market]) : ("D" + overseas::exchan_code[market]);
    
    json::object header = api::kis::default_ws_header;
    header["tr_type"] = attach ? "1" : "2";
    json::object body = {
        {"tr_id", "HDFSASP0"},
        {"tr_key", m_code + code},
    };
    return api::kis::send_ws(ws_client, header, body);
}

// -------------------------------------------------------------------------------------------------