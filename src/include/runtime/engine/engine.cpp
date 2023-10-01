#include <cmath>
#include <ctime>
#include <vector>
#include <map>
#include <iostream>

#include <boost/json/object.hpp>
#include <boost/json/serialize.hpp>

#include "engine.hpp"
#include "../runtime.hpp"
#include "../../api/api.hpp"

using namespace boost;

using namespace api::kis;

using namespace runtime;
using namespace runtime::param;

/**
 * First of all, use domestic market.
*/
int transaction::refresh(string& code) {
    pair<string, string> start_and_today = util::required_period();

    pair<json::object, json::object> response = domestic::stock_chartprice(code, start_and_today.first, start_and_today.second);
    json::object output1 = response.second["output1"].as_object();
    
    indicator::CLOSE = atof(output1["stck_prpr"].as_string().c_str());
    indicator::PER = atof(output1["per"].as_string().c_str());
    indicator::PBR = atof(output1["pbr"].as_string().c_str());

    json::array output2 = response.second["output2"].as_array();
    // AVG
    double price_sum = 0;
    // RSI
    double diff = 0;
    pair<double, int> up_sum = {0, 0};
    pair<double, int> down_sum = {0, 0};
    double alpha = 0.999;
    // BOLLINGER BAND
    double diff_square_sum = 0;

    int idx = 1;
    for(auto& it : output2) {
        json::object data = it.as_object();
        
        price_sum += atof(data["stck_clpr"].as_string().c_str());
        if((diff = atof(data["prdy_vrss"].as_string().c_str())) > 0) {
            up_sum.first += diff;
            up_sum.second++;
        } else {
            down_sum.first += diff;
            down_sum.second++;
        }
        diff_square_sum += diff * diff;
        
        if(idx == 5)
            indicator::AVG_5 = price_sum / 5;
        else if(idx == 20) {
            indicator::AVG_20 = price_sum / 20;
            double variance = (float) diff_square_sum / 20;
            double std = sqrt(variance);
            indicator::BOLLINGER_LOW = indicator::AVG_20 - std * 2;
            indicator::BOLLINGER_HIGH = indicator::AVG_20 + std * 2;
        } else if(idx == 60)
            indicator::AVG_60 = price_sum / 60;
        else if(idx == 9)
            indicator::RSI_9 = (up_sum.first / up_sum.second) / ((up_sum.first / up_sum.second) - (down_sum.first / down_sum.second));
        else if(idx == 14)
            indicator::RSI_14 = (up_sum.first / up_sum.second) / ((up_sum.first / up_sum.second) - (down_sum.first / down_sum.second));
        else if(idx == 28)
            indicator::RSI_28 = (up_sum.first / up_sum.second) / ((up_sum.first / up_sum.second) - (down_sum.first / down_sum.second));

        
        idx++;
        if(idx > 60)
            break;
    }

    cout << code << endl;
    cout << indicator::OPEN << endl;
    cout << indicator::HIGH << endl;
    cout << indicator::LOW << endl;
    cout << indicator::CLOSE << endl;
    cout << indicator::PER << endl;
    cout << indicator::PBR << endl;
    cout << indicator::AVG_5 << endl;
    cout << indicator::AVG_20 << endl;
    cout << indicator::AVG_60 << endl;
    cout << indicator::RSI_9 << endl;
    cout << indicator::RSI_14 << endl;
    cout << indicator::RSI_28 << endl;
    cout << indicator::BOLLINGER_LOW << endl;
    cout << indicator::BOLLINGER_HIGH << endl;
    return 0;
}

/**
 * 
*/
void transaction::run(vector<string>& target_list) {
    map<string, vector<float>> stock_scores;
    map<int, string, greater<int>> buy_list;
    vector<pair<string, int>> sell_list;

    int qty = 0;
    for(auto& code : target_list) {
        int ret = refresh(code);
        if(ret == -1)
            break;
        else if(ret == -2)
            continue;
        
        vector<float> scores = strategy::v0();
        stock_scores[code] = scores;
        int score = (int) accumulate(scores.begin(), scores.end(), 0);
        if(score > 0)
            buy_list[score] = code;
        else if( (qty = util::how_many_have(code)) )
            sell_list.push_back({code, qty});
    }

    for(auto& it : sell_list)
        domestic::sell(it.first, it.second);
    for(auto& it : buy_list) {
        qty = 0;
        domestic::order_cash(true, it.second, qty);
    }
    
    util::show_account();
}

/**
 * 
*/
void transaction::realtime(vector<string>& ccnl_list, vector<string>& ask_list) {

}