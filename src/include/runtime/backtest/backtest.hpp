#include <array>
#include <vector>
#include <string>
#include <map>

#include <boost/json/object.hpp>

using namespace std;

namespace backtest {
    
    bool refresh(string& date, string& code);
    void run(string& start, string& end, int init_cash, vector<string>& target_list);

    namespace util {
        bool tm_is_equal(tm& day1, tm& day2);
        void tm_inc(tm& day);
        string tm_to_string(tm& tm, string format="%Y%m%d");
        bool isWeekend(tm& today);

        void buysell(string& date, multimap<float, pair<string, int>>& scored_list);
        
        void refresh_account();
        void show_account();
        void clear_account();
    };

    namespace env {
        extern map<string, std::array<int, 3>> account;
        extern int cash;
        extern int total_evaluate;
        
        extern vector<string> tr_log;
    };
};