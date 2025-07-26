#include <string>
#include "fadeOutAnimation.hpp"

FadeOutAnimation::FadeOutAnimation(float minBound, float maxBound, float step)
    : m_minBound(minBound),
      m_maxBound(maxBound),
      m_step(step),
      m_currentValue(maxBound),
      m_completed(false) {
}

void FadeOutAnimation::process() {
    if (!m_completed) {
        m_currentValue -= m_step;
        if (m_currentValue <= m_minBound) {
            m_completed = true;
            m_currentValue = 0.01F;
        }
    }
}

float FadeOutAnimation::getValue() {
    return m_currentValue;
}

bool FadeOutAnimation::isCompleted() {
    return m_completed;
}


