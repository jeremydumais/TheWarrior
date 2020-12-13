#include <iostream>
#include "gameWindow.hpp"
#include <SDL2/SDL.h>

int main()
{
    GameWindow gameWindow("The Warrior", 0, 0, 1024, 768);
    gameWindow.show();

    while(gameWindow.isAlive()) {
        gameWindow.processEvents();
    }
    return 0;
}