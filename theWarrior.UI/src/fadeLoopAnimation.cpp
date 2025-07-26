#include <string>
#include "fadeLoopAnimation.hpp"

FadeLoopAnimation::FadeLoopAnimation(float minBound, float maxBound, float step)
    : m_minBound(minBound),
      m_maxBound(maxBound),
      m_step(step),
      m_currentValue(maxBound),
      m_increase(false) {
}

void FadeLoopAnimation::process() {
    if (m_increase)
        m_currentValue += m_step;
    else
        m_currentValue -= m_step;
    if (m_currentValue >= m_maxBound) {
        m_increase = false;
    } else if (m_currentValue <= m_minBound) {
        m_increase = true;
    }
}

float FadeLoopAnimation::getValue() {
    return m_currentValue;
}

bool FadeLoopAnimation::isCompleted() {
    return false;
}

