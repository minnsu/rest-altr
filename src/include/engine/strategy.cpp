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
        score += 1 * (10 - indicator::PER);
    else if(indicator::PER > 30)
        score -= 1 * (indicator::PER / 10);
    
    // PBR
    if(indicator::PBR > 3)
        score -= 1 * (indicator::PBR * 2);
    else if(indicator::PBR < 1.2)
        score += 1 * (indicator::PBR * 5);
    
    // Moving average

    // RSI

    // Bollinger band

    return (int) score;
}