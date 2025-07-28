#include "shakingAnimation.hpp"

ShakingAnimation::ShakingAnimation(float power, int bounceCount)
    : m_power(power),
      m_bounceCount(bounceCount),
      m_currentValue(0.0F),
      m_currentTurn(0),
      m_internalStep(0),
      m_completed(false),
      m_increase(true) {
    float divisor = static_cast<float>(bounceCount);
    if (bounceCount == 0) {
        divisor = 1.0F;
    }
    m_powerStep = power / divisor;
}

void ShakingAnimation::process() {
    if (!m_completed) {
        if (m_increase) {
            m_currentValue += m_powerStep * static_cast<float>(m_bounceCount - m_currentTurn);
        } else {
            m_currentValue -= m_powerStep * static_cast<float>(m_bounceCount - m_currentTurn);
        }
        m_internalStep++;
        if (m_internalStep == 1) {
            m_increase = false;
        } else if (m_internalStep == 3) {
            m_increase = true;
        } else if (m_internalStep == 4) {
            m_internalStep = 0;
            m_currentTurn++;
        }
        if (m_currentTurn == m_bounceCount) {
            m_completed = true;
        }
    }
}

float ShakingAnimation::getValue() {
    return m_currentValue;
}

bool ShakingAnimation::isCompleted() {
    return m_completed;
}


