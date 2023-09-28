#include <vector>

using namespace std;

namespace runtime {
    namespace indicator {
        extern double PRICE;
        extern double PER;
        extern double PBR;
        extern double AVG_5;
        extern double AVG_20;
        extern double AVG_60;
        extern double RSI_9;
        extern double RSI_14;
        extern double RSI_28;
        extern double BOLLINGER_LOW;
        extern double BOLLINGER_HIGH;
    };

    namespace param {
        extern const float TAX;
        extern const float CHARGE;

        extern const float MAX_DIST_RATE;
        extern const float PROFIT_CUT;
        extern const float LOSS_CUT;
    };
    namespace strategy {
        vector<float> v0();
    };
};