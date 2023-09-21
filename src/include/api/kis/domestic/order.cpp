#include "../../api.hpp"

using namespace std;

// api::kis::domestic namespace, Order functions ---------------------------------------------------

/**
 * Domestic: Buy stock.
 * @param {string&} code: stock code
 * @param {int} qty: quantity
 * @param {int} price: default=0, then buy with market price. Else, buy with specified price.
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::buy(string& code, int qty, int price) {
    if(price != 0)
        return api::kis::domestic::order_cash(true, code, qty, price, "00");
    else
        return api::kis::domestic::order_cash(true, code, qty);
}

/**
 * Domestic: Sell stock.
 * @param {string&} code: stock code
 * @param {int} qty: quantity
 * @param {int} price: default=0, then sell with market price. Else, sell with specified price.
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::sell(string& code, int qty, int price) {
    if(price != 0)
        return api::kis::domestic::order_cash(false, code, qty, price, "00");
    else
        return api::kis::domestic::order_cash(false, code, qty);
}

/**
 * Domestic: Order stock as cash
 * @param {bool} buy: true=buy, false=sell
 * @param {string&} code: stock code
 * @param {int} qty: quantity
 * @param {int} price: price
 * @param {string} div: market price(default)="01", "00"(specified price)~"16"(FOK) order division
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::order_cash(bool buy, string& code, int qty, int price, string div) {
    string path = "/uapi/domestic-stock/v1/trading/order-cash";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = api::kis::system ? (buy ? "TTTC0802U" : "TTTC0801U") : (buy ? "VTTC0802U" : "VTTC0801U");
    boost::json::object body = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"PDNO", code},
        {"ORD_DVSN", div},
        {"ORD_QTY", to_string(qty)},
        {"ORD_UNPR", to_string(price)},
    };
    return api::kis::post(path, header, body);
}

/**
 * Domestic: Order stock as credit
 * @param {bool} buy: true=buy, false=sell
 * @param {string&} code: stock code
 * @param {string&} crd_type: credit type
 * @param {string&} loan_date: loan date
 * @param {int} qty: quantity
 * @param {int} price: price
 * @param {string} div: market price(default)="01", "00"(specified price)~"16"(FOK) order division
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::order_credit(bool buy, string& code, string& crd_type, string& loan_date, int qty, int price, string div) {
    string path = "/uapi/domestic-stock/v1/trading/order-credit";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = buy ? "TTTC0852U" : "TTTC0851U";
    boost::json::object body = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"PDNO", code},
        {"CRDT_TYPE", crd_type},
        {"LOAN_DT", loan_date},
        {"ORD_DVSN", div},
        {"ORD_QTY", to_string(qty)},
        {"ORD_UNPR", to_string(price)},
    };
    return api::kis::post(path, header, body);
}

/**
 * Domestic: Revise or cancel the order. {revise_cancel: true=cancel, false=revise}, {remain_all: true=all, false=some}
 * @param {bool} revise_cancel: true=cancel, false=revise
 * @param {bool} remain_all: true=all, false=some of remain
 * @param {string&} order: order number
 * @param {int} qty: quantity
 * @param {int} price: price
 * @param {string} div: market price(default)="01", "00"(specified price)~"16"(FOK) order division
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::revise_cancel(bool revise_cancel, bool remain_all, string& order, int qty, int price, string div) {
    string path = "/uapi/domestic-stock/v1/trading/order-rvsecncl";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = api::kis::system ? "TTTC0803U" : "VTTC0803U";
    boost::json::object body = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"KRX_FWDG_ORD_ORGNO", ""},
        {"ORGN_ODNO", order},
        {"ORD_DVSN", div},
        {"RVSE_CNCL_DVSN_CD", (revise_cancel ? "02" : "01")},
        {"ORD_QTY", to_string(qty)},
        {"ORD_UNPR", to_string(price)},
        {"QTY_ALL_ORD_YN", (remain_all ? "Y" : "N")},
    };
    return api::kis::post(path, header, body);
}

/**
 * Domestic: Inquire my daily orders.
 * @param {bool} in3month: if start date is in last 3 month then true, else false.
 * @param {string&} start: start date ex) "20230920"
 * @param {string&} end: end date ex) "20230920"
 * @param {string} cont_fk100: default="", continues=return.second["output"]["CTX_AREA_FK100"]
 * @param {string} cont_nk100: default="", continues=return.second["output"]["CTX_AREA_NK100"]
 * @param {string} code: default="", specifiy the stock code.
 * @param {string} div: all(default)="00", sell="01", buy="02"
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::daily_order(bool in3month, string& start, string& end, string cont_fk100, string cont_nk100, string code, string div) {
    string path = "/uapi/domestic-stock/v1/trading/inquire-daily-ccld";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = api::kis::system ? (in3month ? "TTTC8001R" : "CTSC9115R") : (in3month ? "VTTC8001R" : "VTSC9115R");
    boost::json::object params = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"INQR_STRT_DT", start},
        {"INQR_END_DT", end},
        {"SLL_BUY_DVSN_CD", div},
        {"INQR_DVSN", "01"},
        {"PDNO", code},
        {"CCLD_DVSN", "00"},
        {"ORD_GNO_BRNO", ""},
        {"ODNO", ""},
        {"INQR_DVSN_3", "00"},
        {"INQR_DVSN_1", ""},
        {"CTX_AREA_FK100", cont_fk100},
        {"CTX_AREA_NK100", cont_nk100},
    };
    return api::kis::get(path, header, params);
}

/**
 * Domestic: Inquire stock balance
 * @param {string} cont_fk100: default="", continues=return.second["output"]["CTX_AREA_FK100"]
 * @param {string} cont_nk100: default="", continues=return.second["output"]["CTX_AREA_NK100"]
 * @param {string} div: by stocks(default)="02", by loan dates="01"
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::stock_balance(string cont_fk100, string cont_nk100, string div) {
    string path = "/uapi/domestic-stock/v1/trading/inquire-balance";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = api::kis::system ? "TTTC8434R" : "VTTC8434R";
    boost::json::object params = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"AFHR_FLPR_YN", "N"},
        {"OFL_YN", ""},
        {"INQR_DVSN", div},
        {"UNPR_DVSN", "01"},
        {"FUND_STTL_ICLD_YN", "Y"},
        {"FNCG_AMT_AUTO_RDPT_YN", "N"},
        {"PRCS_DVSN", "00"},
        {"CTX_AREA_FK100", cont_fk100},
        {"CTX_AREA_NK100", cont_nk100},
    };
    return api::kis::get(path, header, params);
}

/**
 * Domestic: Reservation order
 * @param {bool} buy: true=buy, false=sell
 * @param {string&} end_date: reservation end date. maximum after 30 days. ex) "20230920"
 * @param {string&} code: stock code
 * @param {int} qty: quantity
 * @param {int} price: price(default)=0
 * @param {string} div: market price(default)="01", "00"(specified price)~"16"(FOK) order division
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::order_reserve(bool buy, string& end_date, string& code, int qty, int price, string div) {
    string path = "/uapi/domestic-stock/v1/trading/order-resv";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "CTSC0008U";
    boost::json::object body = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"PDNO", code},
        {"ORD_QTY", to_string(qty)},
        {"ORD_UNPR", to_string(price)},
        {"SLL_BUY_DVSN_CD", (buy ? "02" : "01")},
        {"ORD_DVSN_CD", div},
        {"ORD_OBJT_CBLC_DVSN_CD", "10"},
        {"LOAN_DT", ""},
        {"RSVN_ORD_END_DT", end_date},
        {"LDNG_DT", ""},
    };
    return api::kis::post(path, header, body);
}

/**
 * Domestic: Revise or cancel the order. {revise_cancel: true=cancel, false=revise}, {remain_all: true=all, false=some}
 * @param {bool} revise_cancel: true=cancel, false=revise
 * @param {bool} buy: true=buy, false=sell
 * @param {string&} end_date: reservation end date. maximum after 30 days. ex) "20230920"
 * @param {string&} code: stock code
 * @param {string&} order: order number
 * @param {int} qty: quantity
 * @param {int} price: price
 * @param {string} div: market price(default)="01", "00"(specified price)~"16"(FOK) order division
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::revise_cancel_reserve(bool revise_cancel, bool buy, string& end_date, string& code, string& order, int qty, int price, string div) {
    string path = "/uapi/domestic-stock/v1/trading/order-resv-rvsecncl";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = revise_cancel ? "CTSC0009U" : "CTSC0013U";
    boost::json::object body = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"PDNO", code},
        {"ORD_QTY", to_string(qty)},
        {"ORD_UNPR", to_string(price)},
        {"SLL_BUY_DVSN_CD", (buy ? "02" : "01")},
        {"ORD_DVSN_CD", div},
        {"ORD_OBJT_CBLC_DVSN_CD", "10"},
        {"LOAN_DT", ""},
        {"RSVN_ORD_END_DT", end_date},
        {"CTAL_TLNO", ""},
        {"RSVN_ORD_SEQ", ""},
        {"RSVN_ORD_ORGNO", ""},
        {"RSVN_ORD_ORD_DT", ""},
    };
    return api::kis::post(path, header, body);
}

/**
 * Domestic: Inquire reservation orders list
 * @param {string&} start_date: reservation order start date
 * @param {string&} end_date: reservation order end date
 * @param {string&} order_seq: order sequence number 
 * @param {string} code: all(default)="", else specify stock code
 * @param {string} cont_fk200: continue variable. result.seond["output"]["CTX_AREA_FK200"]
 * @param {string} cont_nk200: continue variable. result.seond["output"]["CTX_AREA_NK200"]
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::reserve_order_list(string& start_date, string& end_date, string& order_seq, string code, string cont_fk200, string cont_nk200) {
    string path = "/uapi/domestic-stock/v1/trading/order-resv-ccnl";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "CTSC0004R";
    boost::json::object params = {
        {"RSVN_ORD_ORD_DT", start_date},
        {"RSVN_ORD_END_DT", end_date},
        {"RSVN_ORD_SEQ", order_seq},
        {"TMNL_MDIA_KIND_CD", "00"},
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"PRCS_DVSN_CD", "0"},
        {"CNCL_YN", "Y"},
        {"PDNO", code},
        {"SLL_BUY_DVSN_CD", ""},
        {"CTX_AREA_FK200", cont_fk200},
        {"CTX_AREA_NK200", cont_nk200},
    };
    return api::kis::get(path, header, params);
}

/**
 * Domestic: Inquire evaluate profit loss
 * @param {string} cont_fk100: continue variable. result.seond["output"]["CTX_AREA_FK100"]
 * @param {string} cont_nk100: continue variable. result.seond["output"]["CTX_AREA_NK100"]
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::profit_loss(string cont_fk100, string cont_nk100) {
    string path = "/uapi/domestic-stock/v1/trading/inquire-balance-rlz-pl";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "TTTC8494R";
    boost::json::object body = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"AFHR_FLPR_YN", "N"},
        {"OFL_YN", ""},
        {"INQR_DVSN", "00"},
        {"UNPR_DVSN", "01"},
        {"FUND_STTL_ICLD_YN", "Y"},
        {"FNCG_AMT_AUTO_RDPT_YN", "N"},
        {"PRCS_DVSN", "00"},
        {"COST_ICLD_YN", ""},
        {"CTX_AREA_FK100", cont_fk100},
        {"CTX_AREA_NK100", cont_nk100},
    };
    return api::kis::post(path, header, body);
}

/**
 * Domestic: Inquire account balance.
*/
pair<boost::json::object, boost::json::object> api::kis::domestic::account_balance() {
    string path = "/uapi/domestic-stock/v1/trading/inquire-account-balance";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "CTRP6548R";
    boost::json::object params = {
        {"CANO", api::kis::CANO},
        {"ACNT_PRDT_CD", api::kis::CANO2},
        {"INQR_DVSN_1", ""},
        {"BSPR_BF_DT_APLY_YN", ""},
    };
    return api::kis::get(path, header, params);
}

// ------------------------------------------------------------------------------------------------