#include "valueChangeAnimation.hpp"

ValueChangeAnimation::ValueChangeAnimation(float from, float to, float step)
    : m_to(to),
      m_from(from),
      m_step(step),
      m_currentValue(from),
      m_completed(false) {
}

void ValueChangeAnimation::process() {
    if (!m_completed) {
        m_currentValue -= m_step;
        if (m_currentValue <= m_to) {
            m_completed = true;
            m_currentValue = 0.01F;
        }
    }
}

float ValueChangeAnimation::getValue() const {
    return m_currentValue;
}

bool ValueChangeAnimation::isCompleted() {
    return m_completed;
}


