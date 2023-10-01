#include "backtest.hpp"

map<string, std::array<int, 3>> backtest::env::account;
int backtest::env::cash;
int backtest::env::total_evaluate;

int backtest::env::data_year;
boost::json::object backtest::env::year_data_cache;
string backtest::env::date;

vector<string> backtest::env::tr_log;