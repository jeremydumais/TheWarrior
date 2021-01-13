#include <iostream>
#include "gameWindow.hpp"
#include <GL/glut.h>
#include <SDL2/SDL.h>

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    GameWindow gameWindow("The Warrior", 100, 50, 1024, 900);
    gameWindow.show();

    while(gameWindow.isAlive()) {
        gameWindow.processEvents();
    }
    return 0;
}