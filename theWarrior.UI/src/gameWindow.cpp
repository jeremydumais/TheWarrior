#include <SDL2/SDL_mixer.h>
#include <fmt/format.h>
#include <filesystem>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <boost/bind/placeholders.hpp>
#include "gameWindow.hpp"
#include "gameState.hpp"
#include "gameStateStorage.hpp"
#include "player.hpp"
#include "specialFolders.hpp"
#include "worldState.hpp"

using namespace std;
using namespace thewarrior::models;
namespace fs = std::filesystem;

namespace thewarrior::ui {

GameWindow::GameWindow(const string &title,
        int x, int y,
        int width, int height)
    : m_WindowSize(width, height),
      m_gameMapMode(nullptr),
      m_mainMenuMode(nullptr) {
    if (!initializeOpenGL(title, x, y, width, height)) {
        return;
    }
    if (!initializeAudio()) {
        return;
    }
    if (!loadResourceFiles()) {
        return;
    }
    // Initialize save game repository
    if (!m_controller.initializeSaveGameRepository()) {
        throw std::runtime_error(m_controller.getLastError());
    }
    m_inputDevicesState = std::make_shared<InputDevicesState>();

    SDL_JoystickEventState(SDL_ENABLE);
    SDL_GameControllerEventState(SDL_ENABLE);
    if (SDL_IsGameController(0)) {
        m_gameController = SDL_GameControllerOpen(0);
    } else {
        m_joystick = SDL_JoystickOpen(0);
    }

    subscribeEvents();
    if (!initializeMenu()) return;

    m_fpsCalculator.initialize();
    m_windowSizeChanged(m_WindowSize);
    //HACK: To Remove
    //createNewGame("Jed");
}

GameWindow::~GameWindow() {
    if (m_mainMenuMode) m_mainMenuMode->unloadGLMapObjects();
    if (m_gameMapMode) m_gameMapMode->unloadGLMapObjects();
    if (m_gameController != nullptr) {
        SDL_GameControllerClose(m_gameController);
    }
    if (m_joystick != nullptr) {
        SDL_JoystickClose(m_joystick);
    }
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void GameWindow::show() {
    SDL_ShowWindow(m_window);
}

void GameWindow::hide() {
    SDL_HideWindow(m_window);
}

bool GameWindow::isAlive() const {
    return !m_mustExit;
}

void GameWindow::processEvents() {
    if (m_nextAction) {
        auto action = std::move(*m_nextAction);
        m_nextAction.reset();
        action();
    }
    SDL_Event e;
    if (m_gameController != nullptr) {
        m_inputDevicesState->processGameController(m_gameController);
    } else {
        m_inputDevicesState->processJoystick(m_joystick);
    }
    while (SDL_PollEvent(&e) != 0) {
        m_inputDevicesState->processEvent(e);
        if (e.type == SDL_KEYUP) {
            m_blockKeyDown = false;
        }
        if (e.type == SDL_QUIT) {
            m_mustExit = true;
            continue;
        }
        switch (m_interactionMode) {
            case InteractionMode::MainMenu:
                if (m_mainMenuMode) m_mainMenuMode->processEvents(e);
                break;
            case InteractionMode::Game:
                if (m_gameMapMode) m_gameMapMode->processEvents(e);
                break;
            default:
                break;
        }

        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                int screenWidth = 0, screenHeight = 0;
                SDL_GetWindowSize(m_window, &screenWidth, &screenHeight);
                m_WindowSize.setSize(screenWidth, screenHeight);
                glViewport(0, 0, m_WindowSize.width(), m_WindowSize.height());
                m_windowSizeChanged(m_WindowSize);
            }
        }
    }
    m_inputDevicesState->confirmDirections();
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    if ((keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL]) && keystate[SDL_SCANCODE_F]) {
        if (!m_blockKeyDown) {
            m_toggleFPS = !m_toggleFPS;
            m_blockKeyDown = true;
        }
    }
    switch (m_interactionMode) {
        case InteractionMode::MainMenu:
            if (m_mainMenuMode) m_mainMenuMode->update();
            break;
        case InteractionMode::Game:
            if (m_gameMapMode) m_gameMapMode->update();
            break;
        default:
            break;
    }
    m_windowUpdate(1.0F / 90.0F);
    render();
    if (m_toggleFPS) {
        m_fpsCalculator.calculate();
    }
}

bool GameWindow::initializeOpenGL(const std::string &title,
        int x, int y,
        int width, int height) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        cerr << fmt::format("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Use OpenGL 3.1 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    m_window = SDL_CreateWindow(title.c_str(),
            x,
            y,
            width,
            height,
            //HACK: To Remove and uncomment below
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);
            //SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED);
    if (m_window == nullptr) {
        cerr << fmt::format("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create context
    m_gContext = SDL_GL_CreateContext(m_window);
    if (m_gContext == nullptr) {
        cerr << fmt::format("OpenGL context could not be created! SDL Error: {0}\n", SDL_GetError());
        return false;
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        cerr << fmt::format("Error initializing GLEW! {0}\n", glewGetErrorString(glewError));
        return false;
    }

    // Use Vsync
    if (SDL_GL_SetSwapInterval(-1) < 0) {
        cerr << fmt::format("Warning: Unable to set VSync! SDL Error: {0}\n", SDL_GetError());
        return false;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

bool GameWindow::initializeAudio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        cerr << fmt::format("SDL could not initialize! SDL_Error: {0}\n", SDL_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cerr << fmt::format("SDL_mixer could not initialize! SDL_mixer Error: {0}\n", SDL_GetError());
        return false;
    }
    return true;
}

bool GameWindow::initializeMenu() {
    m_mainMenuMode = std::make_unique<MainMenuMode>();
    if (!m_mainMenuMode->initShaders(m_controller.getResourcesPath())) {
        cerr << m_mainMenuMode->getLastError() << "\n";
        return false;
    }
    m_mainMenuMode->initialize(m_controller.getResourcesPath(),
            m_textService,
            m_inputDevicesState);
    m_windowSizeChanged.connect(boost::bind(&MainMenuMode::gameWindowSizeChanged, m_mainMenuMode.get(), boost::placeholders::_1));
    m_mainMenuMode->quitRequested.connect(boost::bind(&GameWindow::quitRequested, this));
    m_mainMenuMode->newGameRequested.connect(boost::bind(&GameWindow::newGameRequested, this, boost::placeholders::_1));
    m_mainMenuMode->loadGameRequested.connect(boost::bind(&GameWindow::loadGameRequested, this, boost::placeholders::_1));
    m_windowSizeChanged(m_WindowSize);
    return true;
}

bool GameWindow::initializeGame(const GameState &gameState) {
    m_gameMapMode = std::make_unique<GameMapMode>();
    if (!m_gameMapMode->initShaders(m_controller.getResourcesPath())) {
        cerr << m_gameMapMode->getLastError() << "\n";
        return false;
    }
    if (!m_gameMapMode->initialize(m_controller.getResourcesPath(),
            gameState,
            m_textService,
            m_inputDevicesState)) {
        return false;
    }
    m_windowSizeChanged.connect(boost::bind(&GameMapMode::gameWindowSizeChanged, m_gameMapMode.get(), boost::placeholders::_1));
    m_windowUpdate.connect(boost::bind(&GameMapMode::onGameWindowUpdate, m_gameMapMode.get(), boost::placeholders::_1));
    m_gameMapMode->quitRequested.connect(boost::bind(&GameWindow::quitGameRequested, this));
    m_windowSizeChanged(m_WindowSize);
    return true;
}

bool GameWindow::loadResourceFiles() {
    if (!m_textService->initShader(fmt::format("{0}/shaders/text_330_vs.glsl", m_controller.getResourcesPath()),
                fmt::format("{0}/shaders/text_330_fs.glsl", m_controller.getResourcesPath()))) {
        cerr << m_textService->getLastError() << "\n";
        return false;
    }
    if (!m_textService->initFont(fmt::format("{0}/fonts/verdana.ttf", m_controller.getResourcesPath()))) {
        cerr << m_textService->getLastError() << "\n";
        return false;
    }
    m_textureService.setResourcesPath(m_controller.getResourcesPath());
    return true;
}

void GameWindow::subscribeEvents() {
    m_windowSizeChanged.connect(boost::bind(&GLTextService::gameWindowSizeChanged, m_textService, boost::placeholders::_1));
}

void GameWindow::render() {
    switch (m_interactionMode) {
        case InteractionMode::MainMenu:
            if (m_mainMenuMode) {
                m_mainMenuMode->render();
            }
            break;
        case InteractionMode::Game:
            if (m_gameMapMode) {
                m_gameMapMode->render();
            }
            break;
        default:
            break;
    }
    // Display the FPS
    if (m_toggleFPS) {
        m_textService->useShader();
        m_textService->renderText(m_fpsCalculator.getFPSDisplayText(),
                1.0F,                               // X
                static_cast<float>(m_WindowSize.height()) - 24.0F,  // Y
                0.5F,                               // Scale
                glm::vec3(1.0F, 1.0F, 1.0F));       // Color
    }
    SDL_GL_SwapWindow(m_window);
}

template <class Fn>
void GameWindow::setNextAction(Fn&& fn) {
    m_nextAction = std::forward<Fn>(fn);
}

void GameWindow::quitRequested() {
    m_mustExit = true;
    Mix_CloseAudio();
}

void GameWindow::newGameRequested(const std::string &playerName) {
    setNextAction([this, playerName]() {
        createNewGame(playerName);
    });
}

void GameWindow::loadGameRequested(const std::string &fileName) {
    setNextAction([this, fileName]() {
        loadGame(fileName);
    });
}

void GameWindow::quitGameRequested() {
    setNextAction([this]() {
        returnToMainMenu();
    });
}

void GameWindow::createNewGame(std::string playerName) {
    m_windowSizeChanged.disconnect_all_slots();
    m_mainMenuMode->quitRequested.disconnect_all_slots();
    m_mainMenuMode->newGameRequested.disconnect_all_slots();
    m_mainMenuMode.reset();
    m_mainMenuMode = nullptr;
    Player player(playerName);
    WorldState worldState;
    //HACK: Remove this and uncomment below
    worldState.setCurrentMapName("homeHouseV1.map");
    worldState.setPlayerPosition(Point<int>(13, 10));
    player.setFacing(PlayerFacing::Down);
    // worldState.setCurrentMapName("Outworld.map");
    // worldState.setPlayerPosition(Point<int>(21, 25));
    //worldState.setCurrentMapName("kingAldricCastle-OuterBailey.map");
    //worldState.setPlayerPosition(Point<int>(18, 14)); 
    GameState newGameState(player, worldState);
    if (initializeGame(newGameState)) {
        m_interactionMode = InteractionMode::Game;
    } else {
        m_mustExit = true;
        Mix_CloseAudio();
    }
    m_mustCreateNewGame = false;
}

void GameWindow::loadGame(std::string fileName) {
    m_windowSizeChanged.disconnect_all_slots();
    m_mainMenuMode->quitRequested.disconnect_all_slots();
    m_mainMenuMode->newGameRequested.disconnect_all_slots();
    m_mainMenuMode.reset();
    m_mainMenuMode = nullptr;
    Player player("Ragnar");
    WorldState worldState;
    GameState loadedGameState(player, worldState);
    storage::GameStateStorage gameStateStorage;
    try {
        std::string fullPath = fs::path(thewarrior::utils::SpecialFolders::getSaveGameDirectory()) / fileName;
        gameStateStorage.loadGameState(fullPath, loadedGameState);
    } catch (const std::invalid_argument &err) {
        std::cerr << err.what() << '\n';
        m_mustExit = true;
        Mix_CloseAudio();
        return;
    } catch (const std::runtime_error &err) {
        std::cerr << err.what() << '\n';
        m_mustExit = true;
        Mix_CloseAudio();
        return;
    }

    if (initializeGame(loadedGameState)) {
        m_interactionMode = InteractionMode::Game;
    } else {
        m_mustExit = true;
        Mix_CloseAudio();
    }
    m_mustLoadGame = false;
}

void GameWindow::returnToMainMenu() {
    m_windowSizeChanged.disconnect_all_slots();
    m_windowUpdate.disconnect_all_slots();
    m_gameMapMode->quitRequested.disconnect_all_slots();
    m_gameMapMode.reset();
    m_gameMapMode = nullptr;
    if (initializeMenu()) {
        m_interactionMode = InteractionMode::MainMenu;
    } else {
        m_mustExit = true;
        Mix_CloseAudio();
    }
    m_mustReturnToMainMenu = false;
}

}  // namespace thewarrior::ui
