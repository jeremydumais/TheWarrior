#pragma once

#include <algorithm>
#include <cmath>

namespace thewarrior::ui {

inline float battleHitChance(float attack, float defense) {
    if (attack <= 0.0F) {
        return 0.0F;
    }
    if (defense <= 0.0F || attack >= defense) {
        return 0.95F;
    }
    return std::clamp(attack / defense, 0.15F, 0.95F);
}

inline int battleHitDamage(float attack, float effectiveDefense,
                           float criticalBonus, float randomRoll) {
    if (attack <= 0.0F) {
        return 0;
    }
    return std::max(1, static_cast<int>(
        std::ceil(attack * criticalBonus * randomRoll - effectiveDefense)));
}

}  // namespace thewarrior::ui
