#include <iostream>

#include "series_calc.hpp"

using namespace series;

/**
 * Return local extremum point of distribtion.
 * @param {Series&} dist: target series data
 * @return : {N, true} means that dist(N) is local maximum, {N, false} is reverse.
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
 * Return crossing index and direction of distribution 1 and 2.
 * @param {Series&} dist1: target series data
 * @param {Series&} dist2: victim series data
 * @return : {N, true} means that dist1(N) cross the dist2 below to above, {N, false} is reverse.
*/
vector<pair<int, bool>> series::crossing(Series& dist1, Series& dist2, int end_idx) {
    vector<pair<int, bool>> result;
    
    Series diff = dist1 - dist2;
    bool sign = (diff(0) > 0 ? true : false);

    for(int i = 1; i < end_idx; i++) {
        bool before = sign;
        sign = (diff(i) > 0 ? true : false);
        if(before != sign)
            result.push_back({i, sign});
    }
    return result;
}

/**
 * Make target series's mean list by window rolling method.
 * @param {Series&} target: target series data
 * @param {int} window: rolling window size
 * @param {int} min_period: what index to start calculate
*/
Series series::rolling_mean(Series& target, int window, int min_period) {
    Series result = Series::Zero(target.size());
    for(int i = min_period; i < result.size(); i++) {
        result(i) = target.segment(max(i+1-window, 0), min(i+1, window)).mean();
    }
    return result;
}

/**
 * Make target series's standard deviation list by window rolling method.
 * @param {Series&} target: target series data
 * @param {int} window: rolling window size
 * @param {int} min_period: what index to start calculate
*/
Series series::rolling_std(Series& target, int window, int min_period) {
    Series result = Series::Zero(target.size());
    for(int i = min_period; i < result.size(); i++) {
        Series tmp = target.segment(max(i+1-window, 0), min(i+1, window));
        tmp = tmp - Series::Constant(tmp.size(), tmp.mean());
        result(i) = sqrt((tmp.dot(tmp)) / tmp.size());
    }
    return result;
}

/**
 * Make RSI series of change series list by days
 * @param {Series&} change: target series data. changed value series
 * @param {int} days: calculate RSI days before index
*/
Series series::RSI(Series& change, int days) {
    Series result = Series::Zero(change.size());
    
    pair<int, int> up = {0, 0};
    pair<int, int> down = {0, 0};
    int i;
    for(i = 0; i < days; i++) { // initial RSI
        if(change(i) > 0) {
            up.first += change(i);
            up.second++;
        } else if(change(i) < 0) {
            down.first += change(i);
            down.second++;
        }
    }
    double AU = up.first / (1e-6 + up.second);
    double AD = down.first / (1e-6 + down.second);
    result(days-1) = AU / (AU - AD + 1e-6);

    for(;i < change.size(); i++) {
        if(change(i) > 0) {
            AU = ((days - 1) * AU + change(i)) / days;
            AD = ((days - 1) * AD) / days;
        } else if(change(i) < 0) {
            AU = ((days - 1) * AU) / days;
            AD = ((days - 1) * AD + change(i)) / days;
        }
        result(i) = AU / (AU - AD + 1e-6);
    }
    return result;
}