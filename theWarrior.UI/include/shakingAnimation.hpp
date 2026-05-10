#pragma once

#include "IAnimation.hpp"

class ShakingAnimation : public thewarrior::ui::IAnimation {
 public:
    ShakingAnimation(float power, int bounceCount);
    ~ShakingAnimation() override = default;
    void process() override;
    float getValue() const override;
    bool isCompleted() override;


 private:
    float m_power;
    int m_bounceCount;
    float m_powerStep;
    float m_currentValue;
    int m_currentTurn;
    int m_internalStep;
    bool m_completed;
    bool m_increase;
};
