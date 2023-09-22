#include "../../api.hpp"

using namespace api::kis;

// api::kis::overseas namespace, Inquire functions --------------------------------------------------

/**
 * Overseas: Inquire conclude price.
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).
 * @param {string&} code: stock code
 * @param {bool} after_close: false(default), true = inquire us markets price after closed.
*/
pair<json::object, json::object> overseas::ccnl(overseas::market market, string& code, bool after_close) {
    string path = "/uapi/overseas-price/v1/quotations/price";

    string m_code = overseas::exchan_code[market];
    if(after_close)
        m_code = overseas::exchan_code[11 + market];

    json::object header = default_tr_header;
    header["tr_id"] = "HHDFS00000300";
    json::object params = {
        {"AUTH", ""},
        {"EXCD", m_code},
        {"SYMB", code},
    };
    return api::kis::get(path, header, params);
}

/**
 * Overseas: Inquire period price.
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).
 * @param {string&} code: stock code
 * @param {string&} period: day="0", week="1", month="2"
 * @param {string} date: today(default)="", other day="YYYYMMDD"
 * @param {string} cont: ...?
 * @param {bool} after_close: false(default), true = inquire us markets price after closed.
*/
pair<json::object, json::object> overseas::period_price(overseas::market market, string& code, string& period, string date, string cont, bool after_close) {
    string path = "/uapi/overseas-price/v1/quotations/dailyprice";

    string m_code = overseas::exchan_code[market];
    if(after_close)
        m_code = overseas::exchan_code[11 + market];

    json::object header = default_tr_header;
    header["tr_id"] = "HHDFS76240000";
    json::object params = {
        {"AUTH", ""},
        {"EXCD", m_code},
        {"SYMB", code},
        {"GUBN", period},
        {"GUBN", date},
        {"MODP", "0"},
        {"KEYB", cont},
    };
    return api::kis::get(path, header, params);
}

/**
 * Overseas: Inquire chart price(stock, index, currency exchange rate).
 * @param {string&} div: overseas index="N", currency exchange rate="X"
 * @param {string&} code: stock code
 * @param {string&} period: day="0", week="1", month="2"
 * @param {string} start_date: start date. "YYYYMMDD"
 * @param {string} end_date: end date. "YYYYMMDD"
*/
pair<json::object, json::object> overseas::chartprice(string& div, string& code, string& period, string& start_date, string& end_date) {
    string path = "/uapi/overseas-price/v1/quotations/inquire-daily-chartprice";

    json::object header = default_tr_header;
    header["tr_id"] = "FHKST03030100";
    json::object params = {
        {"FID_COND_MRKT_DIV_CODE", div},
        {"FID_INPUT_ISCD", code},
        {"FID_INPUT_DATE_1", start_date},
        {"FID_INPUT_DATE_2", end_date},
        {"FID_PERIOD_DIV_CODE", period},
    };
    return api::kis::get(path, header, params);
}

/**
 * Overseas: Inquire consition_search
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).
 * @param {int} condition_num: condition number what to use. 0: rate, 1: amount of money, 2: volume, 3: PER, 4: EPS, 5: price, 6: stock_count, 7: market capitalization
 * @param {int} min: condition minimum
 * @param {int} max: condition maximum
*/
pair<json::object, json::object> overseas::condition_search(overseas::market market, int condition_num, int start, int end) {
    string path = "/uapi/overseas-price/v1/quotations/inquire-search";

    string conditions[] = {
        "RATE",
        "AMT",
        "VOLUME",
        "PER",
        "EPS",
        "PRICECUR",
        "SHAR",
        "VALX"
    };

    string m_code = overseas::exchan_code[market];

    json::object header = default_tr_header;
    header["tr_id"] = "HHDFS76410000";
    json::object params = {
        {"AUTH", ""},
        {"EXCD", m_code},
        {"KEYB", ""},
    };
    params["CO_YN_" + conditions[condition_num]] = "1";
    params["CO_ST_" + conditions[condition_num]] = to_string(start);
    params["CO_EN_" + conditions[condition_num]] = to_string(end);
    return api::kis::get(path, header, params);
}

/**
 * Overseas: Inquire settlement day.
 * @param {string&} date: inquire holiday after this. "YYYYMMDD"
*/
pair<json::object, json::object> overseas::settlement(string& date) {
    string path = "/uapi/overseas-stock/v1/quotations/countries-holiday";

    json::object header = default_tr_header;
    header["tr_id"] = "CTOS5011R";
    json::object params = {
        {"TRAD_DT", date},
        {"CTX_AREA_FK", ""},
        {"CTX_AREA_NK", ""},
    };
    return api::kis::get(path, header, params);
}

/**
 * Overseas: Inquire detail price
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).
 * @param {int} code: stock code
*/
pair<json::object, json::object> overseas::price(overseas::market market, string& code) {
    string path = "/uapi/overseas-price/v1/quotations/inquire-search";

    string m_code = overseas::exchan_code[market];

    json::object header = default_tr_header;
    header["tr_id"] = "HHDFS76200200";
    json::object params = {
        {"AUTH", ""},
        {"EXCD", m_code},
        {"SYMB", code},
    };
    return api::kis::get(path, header, params);
}

/**
 * Overseas: Inquire minute stock price
 * @param {overseas::market} market: which nation's market(us_nasd, us_nyse, ns_amex, jp_tkse, ch_shaa, ch_szaa, hk_sehk, vn_hase, vn_vnse).
 * @param {int} code: stock code
 * @param {int} period: minute gap. 1 minute(default)="1", "2", "3", ...
*/
pair<json::object, json::object> overseas::minute_price(overseas::market market, string& code, string period) {
    string path = "/uapi/overseas-price/v1/quotations/inquire-time-itemchartprice";

    string m_code = overseas::exchan_code[market];

    json::object header = default_tr_header;
    header["tr_id"] = "HHDFS76950200";
    json::object params = {
        {"AUTH", ""},
        {"EXCD", m_code},
        {"SYMB", code},
        {"NMIN", period},
        {"PINC", "1"},
        {"NEXT", ""},
        {"NREC", "120"},
        {"FILL", ""},
        {"KEYB", ""},
    };
    return api::kis::get(path, header, params);
}

/**
 * Overseas: Inquire minute index price
 * @param {string&} div: overseas index="N", currency exchange rate="X", rate by won="KX"
 * @param {string&} code: index code
 * @param {string} time_div: in time(default)="0", after close="1"
*/
pair<json::object, json::object> overseas::minute_index(string& div, string& code, string time_div) {
    string path = "/uapi/overseas-price/v1/quotations/inquire-time-itemchartprice";

    json::object header = default_tr_header;
    header["tr_id"] = "FHKST03030200";
    json::object params = {
        {"FID_COND_MRKT_DIV_CODE", div},
        {"FID_INPUT_ISCD", code},
        {"FID_HOUR_CLS_CODE", time_div},
        {"FID_PW_DATA_INCU_YN", "Y"},
    };
    return api::kis::get(path, header, params);
}

// --------------------------------------------------------------------------------------------------