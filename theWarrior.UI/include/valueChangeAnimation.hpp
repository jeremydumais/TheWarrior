#pragma once

#include "IAnimation.hpp"

class ValueChangeAnimation : public thewarrior::ui::IAnimation {
 public:
    ValueChangeAnimation(float from, float to, float step);
    ~ValueChangeAnimation() override = default;
    void process() override;
    float getValue() override;
    bool isCompleted() override;


 private:
    float m_to;
    float m_from;
    float m_step;
    float m_currentValue;
    bool m_completed;
};
