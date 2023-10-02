#include <cstdio>

#include "runtime.hpp"

using namespace std;

Series runtime::indicator::OPEN = RowVectorXd::Zero(270);
Series runtime::indicator::HIGH = RowVectorXd::Zero(270);
Series runtime::indicator::LOW = RowVectorXd::Zero(270);
Series runtime::indicator::CLOSE = RowVectorXd::Zero(270);
Series runtime::indicator::VOLUME = RowVectorXd::Zero(270);
Series runtime::indicator::PER = RowVectorXd::Zero(270);
Series runtime::indicator::PBR = RowVectorXd::Zero(270);
Series runtime::indicator::AVG_5 = RowVectorXd::Zero(270);
Series runtime::indicator::AVG_20 = RowVectorXd::Zero(270);
Series runtime::indicator::AVG_60 = RowVectorXd::Zero(270);
Series runtime::indicator::RSI_9 = RowVectorXd::Zero(270);
Series runtime::indicator::RSI_14 = RowVectorXd::Zero(270);
Series runtime::indicator::RSI_28 = RowVectorXd::Zero(270);
Series runtime::indicator::BOLLINGER_LOW = RowVectorXd::Zero(270);
Series runtime::indicator::BOLLINGER_HIGH = RowVectorXd::Zero(270);

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
    
    

    return scores;
}