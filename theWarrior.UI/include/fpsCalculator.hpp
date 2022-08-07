#pragma once

#include <string>

namespace thewarrior::ui {

class FPSCalculator
{
public:
    FPSCalculator();
    void initialize();
    void calculate();
    float getFPS() const;
    const std::string &getFPSDisplayText() const;
private:
    uint32_t m_frameNo;
    uint32_t m_updateTicks;
    uint32_t m_fpsTicks;
    float m_lastFPS;
    std::string m_lastFPSDisplayText;
    void reset();
};

} // namespace thewarrior::ui
