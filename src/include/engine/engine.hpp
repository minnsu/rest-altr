#include <array>
#include <ctime>
#include <map>
#include <string>
#include <vector>

#include <boost/json.hpp>

using namespace std;

namespace indicator {
    extern double PRICE;
    extern double PER;
    extern double PBR;
    extern double AVG_5;
    extern double AVG_20;
    extern double AVG_60;
    extern double RSI_9;
    extern double RSI_14;
    extern double RSI_28;
    extern double BOLLINGER_LOW;
    extern double BOLLINGER_HIGH;
};

namespace transaction {
    int refresh(string& code);
    void run(vector<string>& target_list);
    void realtime(vector<string>& ccnl_list, vector<string>& ask_list);
};

namespace backtest {
    extern float tax;
    extern float charge;
    extern float max_distribute_rate;
    extern int cash;
    extern int total_evaluate;
    extern float profit_cut;
    extern float loss_cut;
    extern map<string, std::array<int, 3>> account;

    extern boost::json::object data_cache;
    extern string date;
    extern vector<string> tr_log;

    int refresh(string& date, string& code);
    void run(string& start, string& end, int cash, vector<string>& target_list);

    namespace utils {
        void load_data(string path);

        bool tm_is_equal(tm& day1, tm& day2);
        void tm_inc(tm& day);

        void buysell(vector<pair<string, pair<vector<float>, int>>>& buy_list, vector<string>& sell_list);
        void refresh_account();
        void show_account();
        
        void clear_account();
    };
};

namespace strategy {
    vector<float> v0();
}