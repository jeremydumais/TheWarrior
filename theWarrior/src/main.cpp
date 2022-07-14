#include "gameWindow.hpp"
#include <SDL2/SDL.h>
#include <iostream>

int main(int, char **)
{
    GameWindow gameWindow("The Warrior", 100, 50, 1024, 900);
    gameWindow.show();

    while(gameWindow.isAlive()) {
        gameWindow.processEvents();
    }
    return 0;
}