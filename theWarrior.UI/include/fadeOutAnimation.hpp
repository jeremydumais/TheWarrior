#pragma once

#include <string>
#include "IAnimation.hpp"

class FadeOutAnimation : public thewarrior::ui::IAnimation {
 public:
    FadeOutAnimation(float minBound, float maxBound, float step);
    ~FadeOutAnimation() override = default;
    void process() override;
    float getValue() override;
    bool isCompleted() override;


 private:
    float m_minBound;
    float m_maxBound;
    float m_step;
    float m_currentValue;
    bool m_completed;
};
