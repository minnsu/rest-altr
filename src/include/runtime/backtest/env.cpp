#include "backtest.hpp"

map<string, std::array<int, 3>> backtest::env::account;
int backtest::env::cash;
int backtest::env::total_evaluate;

vector<string> backtest::env::tr_log;