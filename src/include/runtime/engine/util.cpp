

#include "engine.hpp"
#include "../../api/api.hpp"

using namespace api::kis;

/**
 * 
*/
pair<string, string> transaction::util::required_period() {
    time_t timer = time(nullptr);
    tm* tm = localtime(&timer);

    char today[16];
    strftime(today, sizeof(today), "%Y%m%d", tm);

    tm->tm_mon = (tm->tm_mon + 9) % 12;
    tm->tm_mday = (tm->tm_mday) % 28;
    char start[16];
    strftime(start, sizeof(start), "%Y%m%d", tm);
    
    return {string(start), string(today)};
}

/**
 * 
*/
int transaction::util::how_many_have(string& code) {
    return false;
}

/**
 * 
*/
void transaction::util::show_account() {
    
}
