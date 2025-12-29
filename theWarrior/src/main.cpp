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
    GameWindow gameWindow("The Warrior", 0, 0, 1024, 768);
    gameWindow.show();
    while (gameWindow.isAlive()) {
        gameWindow.processEvents();
    }
    return 0;
}
