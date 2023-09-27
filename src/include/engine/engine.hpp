#include <vector>
#include <string>

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
    void load_data(string path);
    extern boost::json::object data_cache;
    extern vector<string> tr_log;

    int refresh(string& date, string& code);
    void run(string& start, string& end, int cash, vector<string>& target_list);
};

namespace strategy {
    int v0();
}