#include <vector>
#include <string>

#include <boost/json.hpp>

using namespace std;

namespace indicator {
    float RSI(string& code);
    float PER(string& code);
    float PBR(string& code);
};

namespace transaction {
    void engine(vector<string>& target_list);
    void realtime(vector<string>& ccnl_list, vector<string>& ask_list);
};

namespace backtest {
    void load_data(string path);
    extern boost::json::object year_data;
    boost::json::array get_data(string& date, string& code);

    float RSI(string& code);
    float PER(string& code);
    float PBR(string& code);
    void run(string& start, string& end, int cash);
};