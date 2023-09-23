#include "../../api.hpp"

using namespace api::kis;

// api::kis::overseas namespace, Order functions --------------------------------------------------

/**
 * Overseas: Buy stock.
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).: 
 * @param {string&} code: stock code
 * @param {int} qty: quantity
 * @param {int} price: default=0, then buy with market price. Else, buy with specified price.
*/
pair<json::object, json::object> overseas::buy(string& code, int qty, int price) {
    return overseas::order(true, overseas::code2market(code), code, qty, price);
}

/**
 * Overseas: Sell stock.
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).: 
 * @param {string&} code: stock code
 * @param {int} qty: quantity
 * @param {int} price: default=0, then buy with market price. Else, buy with specified price.
*/
pair<json::object, json::object> overseas::sell(string& code, int qty, int price) {
    return overseas::order(false, overseas::code2market(code), code, qty, price);
}

/**
 * Overseas: Order stock
 * @param {bool} buy: true=buy, false=sell
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).
 * @param {string&} code: stock code
 * @param {int} qty: quantity
 * @param {int} price: price
 * @param {string} div: specified price(default)="00", us & buy(LOO"32", LOC"34"), us & sell(MOO"31", LOO"32", MOC"33", LOC"34"), ethers ""
*/
pair<json::object, json::object> overseas::order(bool buy, overseas::market market, string& code, int qty, int price, string div) {
    string path = "/uapi/overseas-stock/v1/trading/order";
    string tr_ids[12] = {
        "TTT1006U", "TTT1002U", // us sell, buy
        "TTS0307U", "TTS0308U", // japan sell, buy
        "TTS1001U", "TTS1002U", // hongkong sell, buy
        "TTS1005U", "TTS0202U", // shanghai sell, buy
        "TTS0304U", "TTS0305U", // simchun sell, buy
        "TTS0310U", "TTS0311U", // vietnam sell, buy
    };
    int idx = -1;
    switch(market) {
    case overseas::market::us_nasd:
    case overseas::market::us_nyse:
    case overseas::market::us_amex:
        idx = 0;
        break;
    case overseas::market::jp_tkse:
        idx = 1;
        break;
    case overseas::market::hk_sehk:
        idx = 2;
        break;
    case overseas::market::ch_shaa:
        idx = 3;
        break;
    case overseas::market::ch_szaa:
        idx = 4;
        break;
    case overseas::market::vn_hase:
    case overseas::market::vn_vnse:
        idx = 5;
        break;
    default:
        break;
    }
    
    json::object header = default_tr_header;
    header["tr_id"] = (api::kis::system ? "T" : "V") + tr_ids[2*idx + buy];
    json::object body = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"OVRS_EXCG_CD", overseas::market_code[market]},
        {"PDNO", code},
        {"ORD_QTY", to_string(qty)},
        {"CTAC_TLNO", " "},
        {"MGCO_APTM_ODNO", " "},
        {"OVRS_ORD_UNPR", to_string(price)},
        {"SLL_TYPE", (buy ? "" : "00")},
        {"ORD_SVR_DVSN_CD", "0"},
        {"ORD_DVSN", div},
    };
    return api::kis::post(path, header, body);
}

/**
 * Overseas: Revise or cancel order
 * @param {bool} revise_cancel: true=cancel, false=revise
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).
 * @param {string&} code: stock code
 * @param {string&} order: order number
 * @param {int} qty: quantity(default)=0
 * @param {int} price: cancel(default)=0, or price
*/
pair<json::object, json::object> overseas::revise_cancel(bool revise_cancel, overseas::market market, string& code, string& order, int qty, int price) {
    string path = "/uapi/overseas-stock/v1/trading/order-rvsecncl";
    string tr_ids[6] = {
        "TTT1004U", // us revise/cancel
        "TTS0309U", // japan revise/cancel
        "TTS1003U", // hongkong revise/cancel
        "TTS0302U", // shanghai cancel
        "TTS0306U", // simchun cancel
        "TTS0312U", // vietnam cancel
    };
    int idx = -1;
    switch(market) {
    case overseas::market::us_nasd:
    case overseas::market::us_nyse:
    case overseas::market::us_amex:
        idx = 0;
        break;
    case overseas::market::jp_tkse:
        idx = 1;
        break;
    case overseas::market::hk_sehk:
        idx = 2;
        break;
    case overseas::market::ch_shaa:
        idx = 3;
        break;
    case overseas::market::ch_szaa:
        idx = 4;
        break;
    case overseas::market::vn_hase:
    case overseas::market::vn_vnse:
        idx = 5;
        break;
    default:
        break;
    }
    
    json::object header = default_tr_header;
    header["tr_id"] = (api::kis::system ? "T" : "V") + tr_ids[idx];
    json::object body = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"OVRS_EXCG_CD", overseas::market_code[market]},
        {"PDNO", code},
        {"ORGN_ODNO", order},
        {"RVSE_CNCL_DVSN_CD", (revise_cancel ? "02" : "01")},
        {"ORD_QTY", to_string(qty)},
        {"OVRS_ORD_UNPR", to_string(price)},
        {"ORD_SVR_DVSN_CD", "0"},
    };
    return api::kis::post(path, header, body);
}


/**
 * Overseas: Reservation order
 * @param {bool} buy: true=buy, false=sell
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).
 * @param {string&} code: stock code
 * @param {int} qty: quantity
 * @param {int} price: price
 * // only us markets
 * @param {string} div: specified price(default)="00", MOO="31"
 * // only others markets.
 * @param {string} order_date: order date.
 * @param {bool} cancel: false(default)=buy/sell, true=cancel.
 * @param {string} order_num: order number for cancel.
*/
pair<json::object, json::object> overseas::order_reserve(bool buy, overseas::market market, string& code, int qty, int price, string div, string order_date, bool cancel, string order_num) {
    string path = "/uapi/overseas-stock/v1/trading/order-resv";
    string tr_ids[12] = {
        "TTT3016U", "TTT3014U", // us sell, buy
        "TTS3013U", // others
    };
    string type;
    int idx = 2;
    switch(market) {
    case overseas::market::us_nasd:
    case overseas::market::us_nyse:
    case overseas::market::us_amex:
        idx = buy ? 1 : 0;
        break;
    case overseas::market::jp_tkse:
        type = "515";
        break;
    case overseas::market::hk_sehk:
        type = "501";
        break;
    case overseas::market::ch_shaa:
        type = "551";
        break;
    case overseas::market::ch_szaa:
        type = "552";
        break;
    case overseas::market::vn_hase:
        type = "507";
        break;
    case overseas::market::vn_vnse:
        type = "508";
        break;
    }
    
    json::object header = default_tr_header;
    header["tr_id"] = (api::kis::system ? "T" : "V") + tr_ids[idx];
    json::object body = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"PDNO", code},
        {"OVRS_EXCG_CD", overseas::market_code[market]},
        {"FT_ORD_QTY", to_string(qty)},
        {"FT_ORD_UNPR3", to_string(price)},
        {"ORD_SVR_DVSN_CD", "0"},
    };
    if(idx == 0) {
        body["ORD_DVSN"] = div;
    } else if(idx == 2) {
        body["PRDT_TYPE_CD"] = type;
        body["SLL_BUY_DVSN_CD"] = buy ? "02" : "01";
        body["RVSE_CNCL_DVSN_CD"] = cancel ? "02" : "00";
        body["OVRS_RSVN_ODNO"] = order_num;
        body["RSVN_ORD_RCIT_DT"] = order_date;
        
    }
    return api::kis::post(path, header, body);
}

/**
 * Overseas: Revise or cancel us reservation order. If you want cancel others market, then use order_reserve with other markets parameter and cancel=true.
 * @param {string&} order_date: order date
 * @param {string&} order_num: order_num
*/
pair<json::object, json::object> overseas::cancel_reserve(string& order_date, string& order_num) {
    string path = "/uapi/overseas-stock/v1/trading/order-resv-ccnl";
    
    json::object header = default_tr_header;
    header["tr_id"] = api::kis::system ? "TTTT3017U" : "VTTT3017U";
    json::object body = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"RSYN_ORD_RCTT_DT", order_date},
        {"OVRS_RSVN_ODNO", order_num},
    };
    return api::kis::post(path, header, body);
}

/**
 * Overseas: Inquire stock balance
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).
 * @param {string} cont_fk200: cont_fk200: default="", continues=return.second["output"]["CTX_AREA_FK200"]
 * @param {string} cont_nk200: cont_nk200: default="", continues=return.second["output"]["CTX_AREA_NK200"]
*/
pair<json::object, json::object> overseas::stock_balance(overseas::market market, string cont_fk200, string cont_nk200) {
    string path = "/uapi/overseas-stock/v1/trading/inquire-balance";
    string currency[5] = {
        "USD",
        "JPY",
        "HKD",
        "CNY",
        "VND"
    };
    int idx = -1;
    switch(market) {
    case overseas::market::us_nasd:
    case overseas::market::us_nyse:
    case overseas::market::us_amex:
        idx = 0;
        break;
    case overseas::market::jp_tkse:
        idx = 1;
        break;
    case overseas::market::hk_sehk:
        idx = 2;
        break;
    case overseas::market::ch_shaa:
    case overseas::market::ch_szaa:
        idx = 3;
        break;
    case overseas::market::vn_hase:
    case overseas::market::vn_vnse:
        idx = 4;
        break;
    }

    json::object header = default_tr_header;
    header["tr_id"] = (api::kis::system ? "TTTS3012R" : "VTTS3012R");
    json::object params = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"OVRS_EXCG_CD", overseas::market_code[market]},
        {"TR_CRCY_CD", currency[idx]},
    };
    return api::kis::get(path, header, params);
}


/**
 * Overseas: Order us stock after closed(10AM ~ 4PM in korea).
 * @param {bool} buy: true=buy, false=sell
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex).
 * @param {string&} code: stock code
 * @param {int} qty: quantity
 * @param {int} price: price
*/
pair<json::object, json::object> overseas::order_after_closed(bool buy, overseas::market market, string& code, int qty, int price) {
    string path = "/uapi/overseas-stock/v1/trading/daytime-order";
    
    json::object header = default_tr_header;
    header["tr_id"] = buy ? "TTTS6036U" : "TTTS6037U";
    json::object body = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"OVRS_EXCG_CD", overseas::market_code[market]},
        {"PDNO", code},
        {"ORD_QTY", to_string(qty)},
        {"OVRS_ORD_UNPR", to_string(price)},
        {"CTAC_TLNO", " "},
        {"MGCO_APTM_ODNO", " "},
        {"ORD_SVR_DVSN_CD", "0"},
        {"ORD_DVSN", "00"},
    };
    return api::kis::post(path, header, body);
}

/**
 * Overseas: Revise or cancel us stock order after closed(10AM ~ 4PM in korea).
 * @param {bool} revise_cancel: true=cancel, false=revise
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex).
 * @param {string&} code: stock code
 * @param {string&} order: order number
 * @param {int} qty: quantity
 * @param {int} price: price
*/
pair<json::object, json::object> overseas::revise_cancel_after_closed(bool revise_cancel, overseas::market market, string& code, string& order, int qty, int price) {
    string path = "/uapi/overseas-stock/v1/trading/daytime-order-rvsecncl";
    
    json::object header = default_tr_header;
    header["tr_id"] = "TTTS6038U";
    json::object body = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"OVRS_EXCG_CD", overseas::market_code[market]},
        {"PDNO", code},
        {"ORGN_ODNO", order},
        {"RVSE_CNCL_DVSN_CD", (revise_cancel ? "02" : "01")},
        {"ORD_QTY", to_string(qty)},
        {"OVRS_ORD_UNPR", to_string(price)},
        {"CTAC_TLNO", " "},
        {"MGCO_APTM_ODNO", " "},
        {"ORD_SVR_DVSN_CD", "0"},
    };
    return api::kis::post(path, header, body);
}

/**
 * Overseas: Inquire profit loss in period.
 * @param {string&} start_date: start date
 * @param {string&} end_date: end date
 * @param {string} curreny: each currency(default)="01", korean won="02"
 * @param {string} cont_fk200: cont_fk200: default="", continues=return.second["output"]["CTX_AREA_FK200"]
 * @param {string} cont_nk200: cont_nk200: default="", continues=return.second["output"]["CTX_AREA_NK200"]
*/
pair<json::object, json::object> overseas::period_profit_loss(string& start_date, string& end_date, string currency, string cont_fk200, string cont_nk200) {
    string path = "/uapi/overseas-stock/v1/trading/inquire-period-profit";
    
    json::object header = default_tr_header;
    header["tr_id"] = "TTTS3039R";
    json::object params = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"OVRS_EXCG_CD", ""},
        {"NATN_CD", ""},
        {"CRCY_CD", ""},
        {"PDNO", ""},
        {"INQR_STRT_DT", start_date},
        {"INQR_END_DT", end_date},
        {"WCRC_FRCR_DVSN_CD", currency},
        {"CTX_AREA_FK200", cont_fk200},
        {"CTX_AREA_NK200", cont_nk200},
    };
    return api::kis::get(path, header, params);
}

// ------------------------------------------------------------------------------------------------