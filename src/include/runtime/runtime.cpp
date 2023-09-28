#include <cstdio>

#include "runtime.hpp"

using namespace std;

double runtime::indicator::PRICE;
double runtime::indicator::PER;
double runtime::indicator::PBR;
double runtime::indicator::AVG_5;
double runtime::indicator::AVG_20;
double runtime::indicator::AVG_60;
double runtime::indicator::RSI_9;
double runtime::indicator::RSI_14;
double runtime::indicator::RSI_28;
double runtime::indicator::BOLLINGER_LOW;
double runtime::indicator::BOLLINGER_HIGH;

const float runtime::param::TAX = 0.002;
const float runtime::param::CHARGE = 0.00015;

const float runtime::param::MAX_DIST_RATE = 0.3;
const float runtime::param::PROFIT_CUT = 0.2;
const float runtime::param::LOSS_CUT = 0.03;

vector<float> runtime::strategy::v0() {
    // printf("┌-----------------------┐\n"
    //         "| Price: %f\n"
    //         "| PER: %f\n"
    //         "| PBR: %f\n"
    //         "| 5 avg: %f\n"
    //         "| 20 avg: %f\n"
    //         "| 60 avg: %f\n"
    //         "| RSI 9: %f\n"
    //         "| RSI 14: %f\n"
    //         "| RSI 28: %f\n"
    //         "| Low BB: %f\n"
    //         "| High BB: %f\n"
    //         "└-----------------------┘\n",
    //     indicator::PRICE,
    //     indicator::PER,
    //     indicator::PBR,
    //     indicator::AVG_5,
    //     indicator::AVG_20,
    //     indicator::AVG_60,
    //     indicator::RSI_9,
    //     indicator::RSI_14,
    //     indicator::RSI_28,
    //     indicator::BOLLINGER_LOW,
    //     indicator::BOLLINGER_HIGH
    // );
    vector<float> scores;
    
    // PER
    if(indicator::PER < 10)
        scores.push_back(10 - indicator::PER);
    else if(indicator::PER > 30)
        scores.push_back(-1 * (indicator::PER / 5));
    else
        scores.push_back(0);
    
    // PBR
    if(indicator::PBR < 1.3)
        scores.push_back(indicator::PBR * 10);
    else if(indicator::PBR > 3)
        scores.push_back(-1 * indicator::PBR * 5);
    else
        scores.push_back(0);
    
    // Moving average
    if(indicator::AVG_5 < indicator::AVG_20 && indicator::RSI_9 > 0.7)
        scores.push_back(10);
    else if(indicator::AVG_5 > indicator::AVG_20 && indicator::RSI_9 < 0.3)
        scores.push_back(-10);

    if(indicator::AVG_20 < indicator::AVG_60 && indicator::RSI_28 > 0.7)
        scores.push_back(10);
    else if(indicator::AVG_20 > indicator::AVG_60 && indicator::RSI_28 < 0.3)
        scores.push_back(-10);

    // RSI
    if(indicator::RSI_14 < 0.3)
        scores.push_back(indicator::RSI_14 * 20);
    else if(indicator::RSI_14 > 0.7)
        scores.push_back(-1 * indicator::RSI_14 * 10);
    
    if(indicator::RSI_9 < indicator::RSI_14 && indicator::RSI_14 < 0.3)
        scores.push_back(indicator::RSI_9 * 20);
    else if(indicator::RSI_9 > indicator::RSI_14 && indicator::RSI_14 > 0.7)
        scores.push_back(-1 * indicator::RSI_9 * 10);

    // Bollinger band
    if(indicator::PRICE <= indicator::BOLLINGER_LOW)
        scores.push_back(7);
    else if(indicator::PRICE >= indicator::BOLLINGER_HIGH * 0.98)
        scores.push_back(-7);
    
    if(indicator::BOLLINGER_HIGH - indicator::BOLLINGER_LOW < indicator::PRICE * 0.1) {
        if(indicator::AVG_20 - indicator::BOLLINGER_LOW > indicator::PRICE - indicator::BOLLINGER_LOW)
            scores.push_back(7);
        else if(indicator::BOLLINGER_HIGH - indicator::AVG_20 > indicator::BOLLINGER_HIGH - indicator::PRICE)
            scores.push_back(-7);
    }

    // Integrity Powerful
    // if(indicator::PRICE < indicator::BOLLINGER_LOW && indicator::RSI_14 < 0.3)
    //     scores.push_back(10);

    return scores;
}