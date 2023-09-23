#include <vector>
#include <string>

using namespace std;

namespace indicator {
    int RSI(string& code);
    int PER(string& code);
    int EPS(string& code);
};

namespace transaction {
    void engine(vector<string>& target_list);
    void realtime(vector<string>& ccnl_list, vector<string>& ask_list);
};