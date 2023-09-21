#include "../../api.hpp"

using namespace std;

// api::kis::domestic namespace, Inquire functions ------------------------------------------------

/**
 * Domestic: Inquire price.
 * @param {string&} code: stock code
 */
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

/**
 * Domestic: Inquire concluded price.
 * @param {string&} code: stock code
 */ 
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

/**
 * Domestic: Inquire period price.
 * @param {string&} code: stock code
 * @param {string} period: day(default)="D", month="M", year="Y"
 */
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

/**
 * Domestic: Inquire asking price and expect conclude price.
 * @param {string&} code: stock code
 */
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

/**
 * Domestic: Inquire stock's inverstor.
 * @param {string&} code: stock code
 */
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

/**
 * Domestic: Inquire stock's member companies.
 * @param {string&} code: stock code
 */
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

/**
 * Domestic: Inquire elw price.
 * @param {string&} code: elw code
 */
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

/**
 * Domestic: Inquire stock chart price
 * @param {string&} code: stock code
 * @param {string&} start: start date ex) "20230919"
 * @param {string&} end: end date ex) "20230919"
 * @param {string} period: day(default)="D", week="W", month="M", year="Y"
 */
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

/**
 * Domestic: Inquire index chart price
 * @param {string&} code: index code
 * @param {string&} start: start date ex) "20230919"
 * @param {string&} end: end date ex) "20230919"
 * @param {string} period: day(default)="D", week="W", month="M", year="Y"
 */
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

/**
 * Domestic: Inquire price at todays specified time.
 * @param {string&} code: stock code
 * @param {string&} time: time ex) "101112" is HHMMSS
 */
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

/**
 * Domestic: Inquire concluded price after market closed.
 * @param {string&} code: stock code
 */
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

/**
 * Domestic: Inquire daily price after market closed.
 * @param {string&} code: stock code
 */
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

/**
 * Domestic: Inquire todays minute price.
 * @param {string&} code: code
 * @param {string&} time: if code is stocks, then "101112" is HHMMSS. Else code is index, then "60" or "120", that is period secs from now to past.
 */
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

/**
 * Domestic: Inquire stocks information.
 * @param {string&} code: stock code
 * @param {string&} type: stock type ex) stock: "300", NASDAQ: "512" ...
 */
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

/**
 * Domestic: Inquire holiday
 * @param {string&} date: inquire after this value. ex) "20230919"
 */
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

/**
 * Domestic: Inquire expect foreign and institution's list.
 * @param {string&} code: stock code
 * @param {string} sort: sort by quantity="0", sort by money volume(default)="1"
 * @param {string} buy_sell: buy(default)="0", sell="1"
 * @param {string} group: total(default)="0", foreign="1", institution="2", etc="3"
 */
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

/**
 * Domestic: Get list of search by condition.
 */
pair<boost::json::object, boost::json::object> api::kis::domestic::condition_search_list() {
    string path = "/uapi/domestic-stock/v1/quotations/psearch-title";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "HHKST03900300";
    boost::json::object params = {
        {"user_id", api::kis::htsID},
    };
    return api::kis::get(path, header, params);
}

/**
 * Domestic: Get result of search by condition.
 * @param {string&} seq: output["seq"] of result of api::kis::domestic::condition_search_list
 */
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

/**
 * Domestic: Inquire stock's program trading trends.
 * @param {string&} code: stock code
 */
pair<boost::json::object, boost::json::object> api::kis::domestic::program_trade(string& code) {
    string path = "/uapi/domestic-stock/v1/quotations/program-trade-by-stock";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "FHPPG04650100";
    boost::json::object params = {
        {"fid_input_iscd", code},
    };
    return api::kis::get(path, header, params);
}

/**
 * Domestic: Inquire trade volume ranking.
 * @param {string&} sort: average volume="0", rate of trade increase="1", average spin rate="2", amount of money="3", average spin rate of money="4"
 * @param {string} target: 9 digits (0 or 1) ex) 000000000 ~ 111111111, each is 증거금 30%, 40%, 50%, 60%, 100%, 신용보증금 30% 40% 50% 60%
 * @param {string} min_price: minimum price, default="", no limit.
 * @param {string} max_price: maximum price, default="", no limit.
 * @param {string} min_volume: minimum volume, default="", no limit.
 */
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

/**
 * Domestic: Inquire estimate trend of foreign and institution about stock.
 * @param {string&} code: stocke code
 */
pair<boost::json::object, boost::json::object> api::kis::domestic::foreign_institution_by_stock(string& code) {
    string path = "/uapi/domestic-stock/v1/quotations/investor-trend-estimate";
    
    boost::json::object header = api::kis::default_tr_header;
    header["tr_id"] = "HHPTJ04160200";
    boost::json::object params = {
        {"MKSC_SHRN_ISCD", code},
    };
    return api::kis::get(path, header, params);
}

/**
 * Domestic: Inquire index minute price.
 * @param {string&} code: index code
 * @param {string} period: 1 min="30" or "60", 10 min="600", 1 hour="3600"
 */
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