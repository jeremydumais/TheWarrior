#pragma once

#include <SDL2/SDL.h>
#include <string>

class GameWindow
{
public:
    GameWindow(const std::string &title,
               int x, int y,
               int width, int height);
    ~GameWindow();
    void show();
    void hide();
    bool isAlive() const;
    void processEvents();
private:
    SDL_Window *window;
    bool mustExit;
};