#include "fpsCalculator.hpp"
#include <SDL2/SDL.h>

FPSCalculator::FPSCalculator()
    : frameNo(0),
      updateTicks(0),
      fpsTicks(0),
      lastFPS(0.0f)
{}

void FPSCalculator::initialize() 
{
    updateTicks = SDL_GetTicks();
    fpsTicks = SDL_GetTicks();
}

void FPSCalculator::calculate() 
{
    frameNo++;
    if( SDL_GetTicks() - updateTicks > 1000 )
    {
        lastFPS = frameNo / ((static_cast<float>(SDL_GetTicks()) - static_cast<float>(fpsTicks)) / 1000.0f);
        reset();
    }
}

float FPSCalculator::getFPS() const
{
    return lastFPS;
}

void FPSCalculator::reset() 
{
    frameNo = 0;
    updateTicks = SDL_GetTicks();
    fpsTicks = SDL_GetTicks();
}
