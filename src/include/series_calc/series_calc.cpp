#include <iostream>

#include "series_calc.hpp"

using namespace series;
/**
 * 
*/
vector<pair<int, bool>> series::local_extremum(Series& dist) {
    vector<pair<int, bool>> extremum;
    int direction = 0;
    for(int i = 1; i < dist.size(); i++) {
        int before = direction;
        if(dist(i) > dist(i-1))
            direction = 1;
        else if (dist(i) < dist(i-1))
            direction = -1;
        
        if(before != direction) {
            if(direction > 0)
                extremum.push_back({i, true});
            else
                extremum.push_back({i, false});
        }
    }
    return extremum;
}

/**
 * 
*/
vector<pair<int, bool>> series::crossing(Series& dist1, Series& dist2) {
    vector<pair<int, bool>> result;
    
    Series diff = dist1 - dist2;
    bool sign = (diff(0) > 0 ? true : false);

    for(int i = 1; i < dist1.size(); i++) {
        bool before = sign;
        sign = (diff(i) > 0 ? true : false);
        if(before != sign)
            result.push_back({i, sign});
    }
    return result;
}

Series series::rolling_mean(Series& target, int window, int min_period) {
    Series result = Series::Zero(target.size());
    for(int i = min_period; i < result.size(); i++) {
        result(i) = target.segment(max(i+1-window, 0), min(i+1, window)).mean();
    }
    return result;
}

Series series::rolling_std(Series& target, int window, int min_period, string code) {
    Series result = Series::Zero(target.size());
    for(int i = min_period; i < result.size(); i++) {
        Series tmp = target.segment(max(i+1-window, 0), min(i+1, window));
        tmp = tmp - Series::Constant(tmp.size(), tmp.mean());
        result(i) = sqrt((tmp.dot(tmp)) / tmp.size());
    }
    return result;
}