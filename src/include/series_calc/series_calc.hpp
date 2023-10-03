#include <vector>

#include <eigen3/Eigen/Dense>

using namespace std;
using namespace Eigen;


namespace series {
    typedef RowVectorXd Series;
    
    vector<pair<int, bool>> local_extremum(Series& dist);
    vector<pair<int, bool>> crossing(Series& dist1, Series& dist2);

    Series rolling_mean(Series& target, int window, int min_period);
    Series rolling_std(Series& target, int window, int min_period, string code);
};