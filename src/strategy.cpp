#include "include/runtime/runtime.hpp"

float runtime::strategy(string& code) {
    float score = 0;
    
    double per = indicator::PER->operator()(cache[code].last_idx);
    double pbr = indicator::PBR->operator()(cache[code].last_idx);

    if(per < 10)
        score += 3 * per;
    else if(per > 30)
        score -= per / 5;
    
    if(pbr < 1.2)
        score += 10 * pbr;
    else if(pbr > 3)
        score -= 4 * pbr;

    return score;
}