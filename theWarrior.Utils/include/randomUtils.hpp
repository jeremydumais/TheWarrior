#pragma once
#include <random>

namespace thewarrior::utils {

class RandomGenerator {
 public:
    static std::mt19937& instance();
};

}  // namespace thewarrior::utils
