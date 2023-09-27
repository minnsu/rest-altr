#include <cstdio>

#include "engine.hpp"

using namespace std;

int strategy::v0() {
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
    float score = 0;
    
    // PER
    if(indicator::PER < 10)
        score += (10 - indicator::PER);
    else if(indicator::PER > 30)
        score -= 1 * (indicator::PER / 10);
    
    // PBR
    if(indicator::PBR < 1.2)
        score += (indicator::PBR * 4);
    else if(indicator::PBR > 3)
        score -= (indicator::PBR * 2);
    
    // Moving average

    // RSI
    if(indicator::RSI_14 < 0.3)
        score += (indicator::RSI_14 * 20);
    else if(indicator::RSI_14 > 0.7)
        score -= (indicator::RSI_14 * 10);
    
    if(indicator::RSI_9 < indicator::RSI_14 && indicator::RSI_14 < 0.3)
        score += (indicator::RSI_9 * 20);
    else if(indicator::RSI_9 > indicator::RSI_14 && indicator::RSI_14 > 0.7)
        score -= (indicator::RSI_9 * 10);

    // Bollinger band
    if(indicator::PRICE <= indicator::BOLLINGER_LOW)
        score += 7;
    else if(indicator::PRICE >= indicator::BOLLINGER_HIGH * 0.98)
        score -= 7;
    
    if(indicator::BOLLINGER_HIGH - indicator::BOLLINGER_LOW < indicator::PRICE * 0.1) {
        if(indicator::AVG_20 - indicator::BOLLINGER_LOW > indicator::PRICE - indicator::BOLLINGER_LOW)
            score += 7;
        else if(indicator::BOLLINGER_HIGH - indicator::AVG_20 > indicator::BOLLINGER_HIGH - indicator::PRICE)
            score -= 7;
    }

    // Integrity Powerful
    if(indicator::PRICE < indicator::BOLLINGER_LOW && indicator::RSI_14 < 0.3)
        score += 10;

    return (int) score;
}