#include <vector>
#include <map>

#include <sqlite3.h>
#include <eigen3/Eigen/Dense>

#include "../series_calc/series_calc.hpp"

using namespace std;

using namespace Eigen;
using namespace series;

namespace runtime {
    namespace indicator {
        
        extern Series* OPEN;
        extern Series* HIGH;
        extern Series* LOW;
        extern Series* CLOSE;
        
        extern Series* VOLUME;
        extern Series* CHANGE;
        extern Series* PER;
        extern Series* PBR;

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
        float v0(string& code);
    };

    
    struct _CACHE {
        string last_date;
        int last_idx;
        array<Series, 8> data;
    };
    extern map<string, _CACHE> cache; // {code: {last_date, last_idx, cached_data} }

    extern sqlite3 *db;
    namespace DB {
        void SQL_OPEN();
        void SQL_CACHING(string& start_date, string& end_date, vector<string>& target_list);
        void SQL_ERROR_CHECK(int rc, char *ErrMsg);
    };
};