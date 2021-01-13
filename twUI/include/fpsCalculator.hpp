#pragma once

class FPSCalculator
{
public:
    FPSCalculator();
    void initialize();
    void calculate();
    float getFPS() const;
private:
    int frameNo;
    int updateTicks;
    int fpsTicks;
    float lastFPS;
    void reset();
};