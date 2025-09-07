#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <memory>
#include <string>
#include "fpsCalculator.hpp"
#include "gameMapMode.hpp"
#include "gameWindowController.hpp"
#include <glm/glm.hpp>
#include "glTextBox.hpp"
#include "glTextService.hpp"
#include "glTextureService.hpp"
#include "inputDevicesState.hpp"
#include "mainMenuMode.hpp"
#include <size.hpp>
#include <tileSize.hpp>
#include <boost/signals2.hpp>

namespace thewarrior::ui {

enum class InteractionMode {
    MainMenu,
    Game
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
    SDL_GLContext m_gContext;
    thewarrior::models::Size<> m_WindowSize;
    boost::signals2::signal<void(const thewarrior::models::Size<> &)> m_windowSizeChanged;
    boost::signals2::signal<void(float deltaTime)> m_windowUpdate;
    bool m_mustExit = false;
    bool m_mustCreateNewGame = false;
    bool m_mustReturnToMainMenu = false;
    InteractionMode m_interactionMode = InteractionMode::Game;
    std::unique_ptr<thewarrior::ui::GameMapMode> m_gameMapMode;
    std::unique_ptr<thewarrior::ui::MainMenuMode> m_mainMenuMode;
    std::string m_executablePath;
    std::shared_ptr<GLTextService> m_textService = std::make_shared<GLTextService>();
    GLTextureService m_textureService;
    // FPS variables
    thewarrior::ui::FPSCalculator m_fpsCalculator;
    bool m_toggleFPS = false;
    // Input
    std::shared_ptr<InputDevicesState> m_inputDevicesState = nullptr;
    bool m_blockKeyDown = false;
    SDL_Joystick *m_joystick = nullptr;
    std::string m_playerName = "Ragnar";
    bool initializeOpenGL(const std::string &title,
                          int x, int y,
                          int width, int height);
    bool initializeAudio();
    bool initializeMenu();
    bool initializeGame(const std::string &playerName);
    bool loadResourceFiles();
    void subscribeEvents();
    void render();
    void quitRequested();
    void newGameRequested(std::string playerName);
    void quitGameRequested();
    void createNewGame();
    void returnToMainMenu();
};

}  // namespace thewarrior::ui
