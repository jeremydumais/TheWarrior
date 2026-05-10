#pragma once

#include "IAnimation.hpp"

class FadeLoopAnimation : public thewarrior::ui::IAnimation {
 public:
    FadeLoopAnimation(float minBound, float maxBound, float step);
    ~FadeLoopAnimation() override = default;
    void process() override;
    float getValue() const override;
    bool isCompleted() override;


 private:
    float m_minBound;
    float m_maxBound;
    float m_step;
    float m_currentValue;
    bool m_increase;
};
