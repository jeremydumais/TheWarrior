#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <map>
#include <memory>
#include <string>
#include "fpsCalculator.hpp"
#include "gameMapMode.hpp"
#include "gameWindowController.hpp"
#include "glPlayer.hpp"
#include <glm/glm.hpp>
#include "glTextBox.hpp"
#include "glTextService.hpp"
#include "glTextureService.hpp"
#include "glTileService.hpp"
#include "inputDevicesState.hpp"
#include <size.hpp>
#include <tileSize.hpp>
#include <boost/signals2.hpp>

namespace thewarrior::ui {

enum class InteractionMode {
    Game,
    NewGame
};

class GameWindow {
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
    thewarrior::ui::controllers::GameWindowController m_controller;
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    SDL_GLContext m_gContext;
    thewarrior::models::Size<> m_WindowSize;
    TileSize m_tileSize = {1.0F, 1.0F, 1.0F};
    boost::signals2::signal<void(const thewarrior::models::Size<> &)> m_windowSizeChanged;
    boost::signals2::signal<void(const TileSize &)> m_tileSizeChanged;
    boost::signals2::signal<void(float deltaTime)> m_windowUpdate;
    bool m_mustExit = false;
    InteractionMode m_interactionMode = InteractionMode::Game;
    thewarrior::ui::GameMapMode m_gameMapMode;
    std::string m_executablePath;
    std::shared_ptr<GLTileService> m_tileService = std::make_shared<GLTileService>();
    std::shared_ptr<GLTextBox> m_textBox = std::make_shared<GLTextBox>();
    std::shared_ptr<GLTextService> m_textService = std::make_shared<GLTextService>();
    GLTextureService m_textureService;
    std::shared_ptr<GLPlayer> m_glPlayer = std::make_shared<GLPlayer>("Ragnar");
    std::map<std::string, unsigned int> m_texturesGLItemStore;
    // FPS variables
    thewarrior::ui::FPSCalculator m_fpsCalculator;
    bool m_toggleFPS = false;
    // Input
    std::shared_ptr<InputDevicesState> m_inputDevicesState = std::make_shared<InputDevicesState>();
    bool m_blockKeyDown = false;
    SDL_Joystick *m_joystick = nullptr;
    bool initializeOpenGL(const std::string &title,
                          int x, int y,
                          int width, int height);
    bool loadResourceFiles();
    void subscribeEvents();
    void render();
    void loadItemStoreTextures();
    void calculateTileSize();
};

}  // namespace thewarrior::ui
