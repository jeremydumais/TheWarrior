#include "fpsCalculator.hpp"
#include <SDL2/SDL.h>
#include <fmt/format.h>

FPSCalculator::FPSCalculator()
    : m_frameNo(0),
      m_updateTicks(0),
      m_fpsTicks(0),
      m_lastFPS(0.0f),
      m_lastFPSDisplayText("")
{}

void FPSCalculator::initialize() 
{
    m_updateTicks = SDL_GetTicks();
    m_fpsTicks = SDL_GetTicks();
}

void FPSCalculator::calculate() 
{
    m_frameNo++;
    if( SDL_GetTicks() - m_updateTicks > 1000 )
    {
        m_lastFPS = static_cast<float>(m_frameNo) / ((static_cast<float>(SDL_GetTicks()) - static_cast<float>(m_fpsTicks)) / 1000.0f);
        m_lastFPSDisplayText = fmt::format("FPS: {0}", static_cast<int>(m_lastFPS));
        reset();
    }
}

float FPSCalculator::getFPS() const
{
    return m_lastFPS;
}

void FPSCalculator::reset() 
{
    m_frameNo = 0;
    m_updateTicks = SDL_GetTicks();
    m_fpsTicks = SDL_GetTicks();
}

const std::string &FPSCalculator::getFPSDisplayText() const
{
    return m_lastFPSDisplayText;
}
