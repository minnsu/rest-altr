#include <vector>

#include <eigen3/Eigen/Dense>

using namespace std;
using namespace Eigen;

typedef RowVectorXd Series;

namespace runtime {
    namespace indicator {
        
        extern Series OPEN;
        extern Series HIGH;
        extern Series LOW;
        extern Series CLOSE;
        
        extern Series VOLUME;
        extern Series PER;
        extern Series PBR;
        extern Series AVG_5;
        extern Series AVG_20;
        extern Series AVG_60;
        extern Series RSI_9;
        extern Series RSI_14;
        extern Series RSI_28;
        extern Series BOLLINGER_LOW;
        extern Series BOLLINGER_HIGH;
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