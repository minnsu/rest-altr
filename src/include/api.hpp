#include <string>

#include <boost/json/object.hpp>

namespace api {
    namespace kis {
        extern std::string htsID;
        extern std::string CANO;
        extern std::string CANO2;
        extern std::string appkey;
        extern std::string appsecret;
        extern std::string token;
        extern std::string approval;
        extern bool system;
        extern std::string domain;
        extern int port;

        extern boost::json::object default_header;
        extern boost::json::object default_tr_header;
        extern boost::json::object default_ws_header;

        void init(int argc, char *argv[]);
        void show_user(bool secret=false);

        std::pair<boost::json::object, boost::json::object> post(std::string& path, boost::json::object& header, boost::json::object& body);
        std::pair<boost::json::object, boost::json::object> get(std::string& path, boost::json::object& header, boost::json::object& params);

        namespace oauth {
            void approval();
            void token(bool revoke=false);
        };

        namespace domestic {
            std::pair<boost::json::object, boost::json::object> price(std::string& code);
            std::pair<boost::json::object, boost::json::object> ccnl(std::string& code);
            std::pair<boost::json::object, boost::json::object> period_price(std::string& code, std::string period="D");
            std::pair<boost::json::object, boost::json::object> ask_ccnl(std::string& code);
            std::pair<boost::json::object, boost::json::object> investor(std::string& code);
            std::pair<boost::json::object, boost::json::object> member(std::string& code);
            std::pair<boost::json::object, boost::json::object> elw_price(std::string& code);
            std::pair<boost::json::object, boost::json::object> stock_chartprice(std::string& code, std::string& start, std::string& end, std::string period="D");
            std::pair<boost::json::object, boost::json::object> index_chartprice(std::string& code, std::string& start, std::string& end, std::string period="D");
            std::pair<boost::json::object, boost::json::object> price_at_time(std::string& code, std::string& time);
            std::pair<boost::json::object, boost::json::object> ccnl_overtime(std::string& code);
            std::pair<boost::json::object, boost::json::object> price_overtime_daily(std::string& code);
            std::pair<boost::json::object, boost::json::object> minute_price(std::string& code, std::string& time);
            std::pair<boost::json::object, boost::json::object> stock_info(std::string& code, std::string& type);
            std::pair<boost::json::object, boost::json::object> holiday(std::string& date);
            std::pair<boost::json::object, boost::json::object> foreign_and_institution(std::string& code, std::string sort="1", std::string buy_sell="0", std::string group="0");
            std::pair<boost::json::object, boost::json::object> condition_search_list();
            std::pair<boost::json::object, boost::json::object> condition_search(std::string& seq);
            std::pair<boost::json::object, boost::json::object> program_trade(std::string& code);
            std::pair<boost::json::object, boost::json::object> volume_rank(std::string& sort, std::string target="111111111", std::string min_price="", std::string max_price="", std::string min_volume="");
            std::pair<boost::json::object, boost::json::object> foreign_institution_by_stock(std::string& code);
            std::pair<boost::json::object, boost::json::object> index_minute_price(std::string& code, std::string period="60");
        };


    };
};