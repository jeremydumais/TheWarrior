#pragma once

#include <string>

class FPSCalculator
{
public:
    FPSCalculator();
    void initialize();
    void calculate();
    float getFPS() const;
    const std::string &getFPSDisplayText() const;
private:
    int frameNo;
    int updateTicks;
    int fpsTicks;
    float lastFPS;
    std::string lastFPSDisplayText;
    void reset();
};