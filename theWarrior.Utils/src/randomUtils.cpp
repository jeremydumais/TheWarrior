#include "randomUtils.hpp"

using thewarrior::utils::RandomGenerator;

std::mt19937& RandomGenerator::instance() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

