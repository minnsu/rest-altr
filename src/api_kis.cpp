#include <iostream>
#include <fstream>

#include <boost/json/serialize.hpp>

#include "include/api.hpp"
#include "include/net.hpp"

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
    {"Content-Type", "utf-8"},
    {"approval_key", ""},
    {"custtype", "P"},
    {"tr_type", ""},
};

// Initialize user data.
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
    }
    api::kis::default_header["Host"] = api::kis::domain;
    api::kis::default_tr_header["Host"] = api::kis::domain;
    api::kis::default_tr_header["appkey"] = api::kis::appkey;
    api::kis::default_tr_header["appsecret"] = api::kis::appsecret;
}

// Show user's data. if parameter is true, then show secret data.
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

// Abstract of net::http::post for KIS Developers API. Return header and body pair.
pair<boost::json::object, boost::json::object> api::kis::post(string& path, boost::json::object& header, boost::json::object& body) {
    string str = net::https::post(api::kis::domain, api::kis::port, path, header, body);
    return net::packet_split(str);
}

// Abstract of net::http::get for KIS Developers API. Return header and body pair.
pair<boost::json::object, boost::json::object> api::kis::get(string& path, boost::json::object& header, boost::json::object& params) {
    string str = net::https::get(api::kis::domain, api::kis::port, path, header, params);
    return net::packet_split(str);
}

// ------------------------------------------------------------------------------------------------


// api::kis::oauth namespace ----------------------------------------------------------------------

// Request and store Realtime(Web socket) approval key.
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

// Request and store access token.
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


// api::kis::domestic namespace -------------------------------------------------------------------

// Domestic: Inquire price.
pair<boost::json::object, boost::json::object> api::kis::domestic::price(string& code) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-price";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHKST01010100";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "J"},
        {"FID_INPUT_ISCD", code},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire concluded price.
pair<boost::json::object, boost::json::object> api::kis::domestic::ccnl(string& code) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-ccnl";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHKST01010300";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "J"},
        {"FID_INPUT_ISCD", code},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire period price.
pair<boost::json::object, boost::json::object> api::kis::domestic::period_price(string& code, string period) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-daily-price";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHKST01010400";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "J"},
        {"FID_INPUT_ISCD", code},
        {"FID_PERIOD_DIV_CODE", period},
        {"FID_ORG_ADJ_PRC", "0"},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire asking price and expect conclude price.
pair<boost::json::object, boost::json::object> api::kis::domestic::ask_ccnl(string& code) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-asking-price-exp-ccn";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHKST01010200";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "J"},
        {"FID_INPUT_ISCD", code},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire stock's inverstor.
pair<boost::json::object, boost::json::object> api::kis::domestic::investor(string& code) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-investor";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHKST01010900";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "J"},
        {"FID_INPUT_ISCD", code},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire stock's member companies.
pair<boost::json::object, boost::json::object> api::kis::domestic::member(string& code) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-member";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHKST01010600";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "J"},
        {"FID_INPUT_ISCD", code},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire elw price.
pair<boost::json::object, boost::json::object> api::kis::domestic::elw_price(string& code) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-investor";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHKEW15010000";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "W"},
        {"FID_INPUT_ISCD", code},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire stock chart price
pair<boost::json::object, boost::json::object> api::kis::domestic::stock_chartprice(string& code, string& start, string& end, string period) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-daily-itemchartprice";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHKST03010100";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "J"},
        {"FID_INPUT_ISCD", code},
        {"FID_INPUT_DATE_1", start},
        {"FID_INPUT_DATE_2", end},
        {"FID_PERIOD_DIV_CODE", period},
        {"FID_ORG_ADJ_PRC", "0"},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire index chart price
pair<boost::json::object, boost::json::object> api::kis::domestic::index_chartprice(string& code, string& start, string& end, string period) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-daily-indexchartprice";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHKUP03500100";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "U"},
        {"FID_INPUT_ISCD", code},
        {"FID_INPUT_DATE_1", start},
        {"FID_INPUT_DATE_2", end},
        {"FID_PERIOD_DIV_CODE", period},
        {"FID_ORG_ADJ_PRC", "0"},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire price at todays specified time.
pair<boost::json::object, boost::json::object> api::kis::domestic::price_at_time(string& code, string& time) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-time-itemconclusion";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHPST01060000";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "J"},
        {"FID_INPUT_ISCD", code},
        {"FID_INPUT_HOUR_1", time},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire concluded price after market closed.
pair<boost::json::object, boost::json::object> api::kis::domestic::ccnl_overtime(string& code) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-time-overtimeconclusion";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHPST02310000";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "J"},
        {"FID_INPUT_ISCD", code},
        {"FID_HOUR_CLS_CODE", "1"},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire daily price after market closed.
pair<boost::json::object, boost::json::object> api::kis::domestic::price_overtime_daily(string& code) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-daily-overtimeprice";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHPST02320000";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "J"},
        {"FID_INPUT_ISCD", code},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire todays minute price.
pair<boost::json::object, boost::json::object> api::kis::domestic::minute_price(string& code, string& time) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-time-itemchartprice";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHKST03010200";
    boost::json::object params = {
        {"FID_ETC_CLS_CODE", ""},
        {"FID_COND_MRKT_DIV_CODE", "J"},
        {"FID_INPUT_ISCD", code},
        {"FID_INPUT_HOUR_1", time},
        {"FID_PW_DATA_INCU_YN", "N"},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire todays minute price.
pair<boost::json::object, boost::json::object> api::kis::domestic::stock_info(string& code, string& type) {
    string path = "/uapi/domestic-stock/v1/quotations/search-info";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "CTPF1604R";
    boost::json::object params = {
        {"PDNO", code},
        {"PRDT_TYPE_CD", type},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire todays minute price.
pair<boost::json::object, boost::json::object> api::kis::domestic::holiday(string& date) {
    string path = "/uapi/domestic-stock/v1/quotations/chk-holiday";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "CTCA0903R";
    boost::json::object params = {
        {"BASS_DT", date},
        {"CTX_AREA_NK", ""},
        {"CTX_AREA_FK", ""},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire expect foreign and institution's list.
pair<boost::json::object, boost::json::object> api::kis::domestic::foreign_and_institution(string& code, string sort, string buy_sell, string group) {
    string path = "/uapi/domestic-stock/v1/quotations/foreign-institution-total";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHPTJ04400000";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "V"},
        {"FID_COND_SCR_DIV_CODE", "16449"},
        {"FID_INPUT_ISCD", code},
        {"FID_DIV_CLS_CODE", sort},
        {"FID_RANK_SORT_CLS_CODE", buy_sell},
        {"FID_ETC_CLS_CODE", group},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Get list of search by condition.
pair<boost::json::object, boost::json::object> api::kis::domestic::condition_search_list() {
    string path = "/uapi/domestic-stock/v1/quotations/psearch-title";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "HHKST03900300";
    boost::json::object params = {
        {"user_id", api::kis::htsID},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Get result of search by condition.
pair<boost::json::object, boost::json::object> api::kis::domestic::condition_search(string& seq) {
    string path = "/uapi/domestic-stock/v1/quotations/psearch-result";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "HHKST03900400";
    boost::json::object params = {
        {"user_id", api::kis::htsID},
        {"seq", seq},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire program trading data.
pair<boost::json::object, boost::json::object> api::kis::domestic::program_trade(string& code) {
    string path = "/uapi/domestic-stock/v1/quotations/program-trade-by-stock";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHPPG04650100";
    boost::json::object params = {
        {"fid_input_iscd", code},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire trade volume ranking.
pair<boost::json::object, boost::json::object> api::kis::domestic::volume_rank(string& sort, string target, string min_price, string max_price, string min_volume) {
    string path = "/uapi/domestic-stock/v1/quotations/volume-rank";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHPST01710000";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "J"},
        {"FID_COND_SCR_DIV_CODE", "20171"},
        {"FID_INPUT_ISCD", "0000"},
        {"FID_DIV_CLS_CODE", "0"},
        {"FID_BLNG_CLS_CODE", sort},
        {"FID_TRGT_CLS_CODE", target},
        {"FID_TRGT_EXLS_CLS_CODE", "111111"},
        {"FID_INPUT_PRICE_1", min_price},
        {"FID_INPUT_PRICE_2", max_price},
        {"FID_VOL_CNT", min_volume},
        {"FID_INPUT_DATE_1", ""},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire estimate of foreign and institution by stock.
pair<boost::json::object, boost::json::object> api::kis::domestic::foreign_institution_by_stock(string& code) {
    string path = "/uapi/domestic-stock/v1/quotations/investor-trend-estimate";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "HHPTJ04160200";
    boost::json::object params = {
        {"MKSC_SHRN_ISCD", code},
    };
    return api::kis::get(path, header, params);
}

// Domestic: Inquire index minute price.
pair<boost::json::object, boost::json::object> api::kis::domestic::index_minute_price(string& code, string period) {
    string path = "/uapi/domestic-stock/v1/quotations/inquire-time-indexchartprice";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHKUP03500200";
    boost::json::object params = {
        {"FID_COND_MRKT_DIV_CODE", "U"},
        {"FID_INPUT_ISCD", code},
        {"FID_INPUT_HOUR_1", period},
        {"FID_PW_DATA_INCU_YN", "N"},
        {"FID_ETC_CLS_CODE", "0"},
    };
    return api::kis::get(path, header, params);
}


// ------------------------------------------------------------------------------------------------