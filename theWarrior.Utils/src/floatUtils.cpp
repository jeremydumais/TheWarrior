#include "floatUtils.hpp"
#include <cmath>

using thewarrior::utils::FloatUtils;

bool FloatUtils::areEqual(float val1, float val2, float epsilon) {
    return std::fabs(val1 - val2) <= epsilon * std::fmax(1.0f, std::fmax(std::fabs(val1), std::fabs(val2)));
}


