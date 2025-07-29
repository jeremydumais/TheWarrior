#include <SDL2/SDL.h>
#include "gameWindow.hpp"

using namespace thewarrior::ui;

int main(int, char **) {
    //HACK: To remove
    GameWindow gameWindow("The Warrior", 0, 0, 1120, 1080);
    //GameWindow gameWindow("The Warrior", 0, 0, 1024, 768);
    gameWindow.show();
    while (gameWindow.isAlive()) {
        gameWindow.processEvents();
    }
    return 0;
}
