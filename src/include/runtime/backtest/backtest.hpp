#include <array>
#include <vector>
#include <string>
#include <map>

#include <boost/json/object.hpp>

using namespace std;

namespace backtest {
    
    int refresh(string& date, string& code);
    void run(string& start, string& end, int init_cash, vector<string>& target_list);

    namespace util {
        void load_data(string path);

        bool tm_is_equal(tm& day1, tm& day2);
        void tm_inc(tm& day);

        void buysell(map<int, pair<string, int>, greater<int>>& buy_list, vector<string>& sell_list, map<string, vector<float>>& scores);
        void refresh_account();
        void show_account();
        void clear_account();
    };

    namespace env {
        extern map<string, std::array<int, 3>> account;
        extern int cash;
        extern int total_evaluate;
        
        extern int data_year;
        extern boost::json::object year_data_cache;
        extern string date;
        
        extern vector<string> tr_log;
    };
};