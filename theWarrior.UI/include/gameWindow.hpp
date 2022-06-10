#pragma once

#include "fpsCalculator.hpp"
#include "gameMap.hpp"
#include "gameMapMode.hpp"
#include "gameWindowController.hpp"
#include "glPlayer.hpp"
#include "glTextBox.hpp"
#include "glTextService.hpp"
#include "glTextureService.hpp"
#include "glTileService.hpp"
#include <size.hpp>
#include <tileSize.hpp>
#include <boost/signals2.hpp>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>

enum class InteractionMode
{
    Game,
    Inventory
};

class GameWindow
{
public:
    GameWindow(const std::string &title,
               int x, int y,
               int width, int height);
    ~GameWindow();
    bool initializeOpenGL(const std::string &title,
                          int x, int y,
                          int width, int height);
    void update(float delta_time);
    void render();
    void show();
    void hide();
    bool isAlive() const;
    void processEvents();
private:
    GameWindowController m_controller;
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    SDL_GLContext m_gContext;
    Size<> m_WindowSize;
    TileSize m_tileSize;
    boost::signals2::signal<void(const Size<> &)> m_WindowSizeChanged;
    bool m_mustExit;
    InteractionMode m_interactionMode;
    GameMapMode m_gameMapMode;
    std::string m_executablePath;
    std::shared_ptr<GLTileService> m_tileService;
    std::shared_ptr<GLTextBox> m_textBox;
    GLTextService m_textService;
    GLTextureService m_textureService;
    std::shared_ptr<GLPlayer> m_glPlayer;
    std::map<std::string, unsigned int> m_texturesGLItemStore;
    //FPS variables
    FPSCalculator m_fpsCalculator;
    bool m_toggleFPS;
    bool m_blockKeyDown;
    SDL_Joystick *m_joystick;
    void loadItemStoreTextures();
    void calculateTileSize();

};

