#include "gameWindow.hpp"
#include <iostream>
#include <fmt/format.h>

using namespace std;

GameWindow::GameWindow(const string &title,
                       int x, int y,
                       int width, int height) 
    : mustExit(false)
{
    //Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cerr << fmt::format("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
	}
	else
	{
		//Create window
		window = SDL_CreateWindow(title.c_str(), 
            x, 
            y, 
            width, 
            height, 
            SDL_WINDOW_HIDDEN);
		if(window == NULL)
		{
			cerr << fmt::format("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return;
		}
		else
		{
			SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
			SDL_UpdateWindowSurface(window);
		}
	}

}

GameWindow::~GameWindow() 
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GameWindow::show() 
{
    SDL_ShowWindow(window);
}

void GameWindow::hide() 
{
    SDL_HideWindow(window);
}

bool GameWindow::isAlive() const
{
    return !mustExit;
}

void GameWindow::processEvents() 
{
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT){
            mustExit = true;
        }
    }
}
