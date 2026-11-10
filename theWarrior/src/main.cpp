#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "gameWindow.hpp"
#include <boost/filesystem.hpp>
#include "../../theWarrior.Utils/include/specialFolders.hpp"

using namespace thewarrior::ui;
using thewarrior::utils::SpecialFolders;

int main(int, char **) {
    // Keep X11 compositing enabled to avoid tearing during map scrolling.
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

    // Create the necessary data folders
    const std::string savesFolder = SpecialFolders::getSaveGameDirectory();

    if (!boost::filesystem::exists(savesFolder)) {
        if (!boost::filesystem::create_directories(savesFolder)) {
            std::cerr << "Unable to create the folder {0}" << savesFolder << std::endl;
            exit(1);
        }
    }
    //HACK: To remove
    //GameWindow gameWindow("The Warrior", 0, 0, 1120, 1080);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize: " << SDL_GetError() << '\n';
        return 1;
    }
    SDL_Rect screenBounds{};
    if (SDL_GetDisplayBounds(0, &screenBounds) < 0) {
        std::cerr << "Unable to get screen dimensions: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }
    GameWindow gameWindow("The Warrior", screenBounds.x, screenBounds.y,
                          screenBounds.w, screenBounds.h);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    if (!gameWindow.isAlive()) {
        std::cerr << "Game window initialization failed. See the error above.\n";
        return 1;
    }
    gameWindow.show();
    while (gameWindow.isAlive()) {
        gameWindow.processEvents();
    }
    return 0;
}
