#pragma once
#include <limits>

namespace thewarrior::utils {

class FloatUtils {
 public:
    static bool areEqual(float val1, float val2, float epsilon = std::numeric_limits<float>::epsilon());
};

}  // namespace thewarrior::utils

