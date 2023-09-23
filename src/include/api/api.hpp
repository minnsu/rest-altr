#include <string>

#include "../net/net.hpp"

using namespace std;
using namespace boost;

namespace api {
    // kis directory
    namespace kis {
        // kis.cpp

        extern string htsID;
        extern string CANO;
        extern string CANO2;
        extern string appkey;
        extern string appsecret;
        extern string token;
        extern string approval;
        extern bool system;
        extern string domain;
        extern int port;
        extern string ws_domain;
        extern int ws_port;

        extern json::object default_header;
        extern json::object default_tr_header;
        extern json::object default_ws_header;

        void init(int argc, char *argv[]);
        void show_user(bool secret=false);

        pair<json::object, json::object> post(string& path, json::object& header, json::object& body);
        pair<json::object, json::object> get(string& path, json::object& header, json::object& params);

        pair<json::object, json::object> send_ws(net::websocket& ws_client, json::object& header, json::object& body);
        string read_ws(net::websocket& ws_client);
        

        namespace oauth {
            void approval();
            void token(bool revoke=false);
        };

        // domestic directory
        namespace domestic {
            // order.cpp

            pair<json::object, json::object> buy(string& code, int qty, int price=0);
            pair<json::object, json::object> sell(string& code, int qty, int price=0);

            pair<json::object, json::object> order_cash(bool buy, string& code, int qty, int price=0, string div="01");
            pair<json::object, json::object> order_credit(bool buy, string& code, string& crd_type, string& loan_date, int qty, int price=0, string div="01");
            pair<json::object, json::object> revise_cancel(bool revise_cancel, bool remain_all, string& order_num, int qty, int price=0, string div="01");
            pair<json::object, json::object> daily_order(bool in3month, string& start, string& end, string cont_fk100="", string cont_nk100="", string code="", string div="00");
            pair<json::object, json::object> stock_balance(string cont_fk100="", string cont_nk100="",string div="02");
            pair<json::object, json::object> order_reserve(bool buy, string& end_date, string& code, int qty, int price=0, string div="01");
            pair<json::object, json::object> revise_cancel_reserve(bool revise_cancel, bool buy, string& end_date, string& code, string& order_num, int qty, int price=0, string div="01");
            pair<json::object, json::object> reserve_order_list(string& start_date, string& end_date, string& order_seq, string code="", string cont_fk200="", string cont_nk200="");
            pair<json::object, json::object> profit_loss(string cont_fk100="", string cont_nk100="");
            pair<json::object, json::object> account_balance();

            // inquire.cpp

            pair<json::object, json::object> price(string& code);
            pair<json::object, json::object> ccnl(string& code);
            pair<json::object, json::object> period_price(string& code, string period="D");
            pair<json::object, json::object> ask_ccnl(string& code);
            pair<json::object, json::object> investor(string& code);
            pair<json::object, json::object> member(string& code);
            pair<json::object, json::object> elw_price(string& code);
            pair<json::object, json::object> stock_chartprice(string& code, string& start, string& end, string period="D");
            pair<json::object, json::object> index_chartprice(string& code, string& start, string& end, string period="D");
            pair<json::object, json::object> price_at_time(string& code, string& time);
            pair<json::object, json::object> ccnl_overtime(string& code);
            pair<json::object, json::object> price_overtime_daily(string& code);
            pair<json::object, json::object> minute_price(string& code, string& time);
            pair<json::object, json::object> stock_info(string& code, string& type);
            pair<json::object, json::object> holiday(string& date);
            pair<json::object, json::object> foreign_and_institution(string& code, string sort="1", string buy_sell="0", string group="0");
            pair<json::object, json::object> condition_search_list();
            pair<json::object, json::object> condition_search(string& seq);
            pair<json::object, json::object> program_trade(string& code);
            pair<json::object, json::object> volume_rank(string& sort, string target="111111111", string min_price="", string max_price="", string min_volume="");
            pair<json::object, json::object> foreign_institution_by_stock(string& code);
            pair<json::object, json::object> index_minute_price(string& code, string period="60");

            // realtime.cpp

            pair<json::object, json::object> conclude_price(net::websocket& ws_client, bool attach, string& code);
            pair<json::object, json::object> asking_price(net::websocket& ws_client, bool attach, string& code);
            pair<json::object, json::object> conclude_notify(net::websocket& ws_client, bool attach);
        };

        // overseas directory
        namespace overseas {
            enum market {
                us_nasd,
                us_nyse,
                us_amex,
                jp_tkse,
                hk_sehk,
                ch_shaa,
                ch_szaa,
                vn_vnse,
                vn_hase,
            };

            // overseas.cpp

            extern string market_code[];
            extern string exchan_code[];

            overseas::market code2market(string& code);

            // order.cpp

            pair<json::object, json::object> buy(overseas::market market, string& code, int qty, int price);
            pair<json::object, json::object> sell(overseas::market market, string& code, int qty, int price);
            
            pair<json::object, json::object> order(bool buy, overseas::market market, string& code, int qty, int price, string div="00");
            pair<json::object, json::object> revise_cancel(bool revise_cancel, overseas::market market, string& code, string& order, int qty=0, int price=0);
            pair<json::object, json::object> order_reserve(bool buy, overseas::market market, string& code, int qty, int price, string div="00", string order_date="", bool cancel=false, string order_num="");
            pair<json::object, json::object> stock_balance(overseas::market market, string cont_fk200="", string cont_nk200="");
            pair<json::object, json::object> order_after_closed(bool buy, overseas::market market, string& code, int qty, int price);
            pair<json::object, json::object> revise_cancel_after_closed(bool revise_cancel, overseas::market market, string& code, string& order, int qty, int price);
            pair<json::object, json::object> cancel_reserve(string& order_date, string& order_num);
            pair<json::object, json::object> period_profit_loss(string& start_date, string& end_date, string currency="", string cont_fk200="", string cont_nk200="");

            // inquire.cpp

            pair<json::object, json::object> ccnl(overseas::market market, string& code, bool after_close);
            pair<json::object, json::object> period_price(overseas::market market, string& code, string& period, string date="", string cont="", bool after_close=false);
            pair<json::object, json::object> chartprice(string& div, string& code, string& period, string& start_date, string& end_date);
            pair<json::object, json::object> condition_search(overseas::market market, int condition_num, int start, int end);
            pair<json::object, json::object> settlement(string& date);
            pair<json::object, json::object> price(overseas::market market, string& code);
            pair<json::object, json::object> minute_price(overseas::market market, string& code, string period="1");
            pair<json::object, json::object> minute_index(string& div, string& code, string time_div="0");

            // realtime.cpp

            pair<json::object, json::object> conclude_price(net::websocket& ws_client, bool attach, overseas::market market, string& code, bool after_close=false);
            pair<json::object, json::object> asking_price_asia(net::websocket& ws_client, bool attach, overseas::market market, string& code);
            pair<json::object, json::object> conclude_notify(net::websocket& ws_client, bool attach);
            pair<json::object, json::object> asking_price_us(net::websocket& ws_client, bool attach, overseas::market market, string& code, bool after_close=false);


        };
    };
};