#include <string>
#include <vector>

using namespace std;

namespace transaction {
    int refresh(string& code);
    void run(vector<string>& target_list);
    void realtime(vector<string>& ccnl_list, vector<string>& ask_list);
};