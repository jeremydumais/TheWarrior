#include <SDL2/SDL_mixer.h>
#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <boost/bind/placeholders.hpp>
#include "gameWindow.hpp"

using namespace std;
using namespace thewarrior::models;

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
    m_inputDevicesState = std::make_shared<InputDevicesState>();

    SDL_JoystickEventState(SDL_ENABLE);
    m_joystick = SDL_JoystickOpen(0);

    subscribeEvents();
    // HACK: to remove (Test only)
    /*m_glPlayer->getInventory()->addItem(m_controller.getItemStore()->findItem("pot001"));
    m_glPlayer->getInventory()->addItem(m_controller.getItemStore()->findItem("ubd001"));
    m_glPlayer->getInventory()->addItem(m_controller.getItemStore()->findItem("hlm001"));
    m_glPlayer->getInventory()->addItem(m_controller.getItemStore()->findItem("swd003"));
    m_glPlayer->getInventory()->addItem(m_controller.getItemStore()->findItem("swd001"));
    m_glPlayer->getInventory()->addItem(m_controller.getItemStore()->findItem("swd002"));
    m_glPlayer->getInventory()->addItem(m_controller.getItemStore()->findItem("shd001"));
    m_glPlayer->getInventory()->addItem(m_controller.getItemStore()->findItem("key001"));
    m_glPlayer->getEquipment().setMainHand(*dynamic_cast<const WeaponItem*>(m_controller.getItemStore()->findItem("swd002").get()));
m_glPlayer->getEquipment().setSecondaryHand(VariantEquipment(*dynamic_cast<const ArmorItem*>(m_controller.getItemStore()->findItem("shd001").get())));
      m_glPlayer->getEquipment().setHead(*dynamic_cast<const ArmorItem*>(m_controller.getItemStore()->findItem("hlm001").get()));
      m_glPlayer->getEquipment().setUpperBody(*dynamic_cast<const ArmorItem*>(m_controller.getItemStore()->findItem("ubd001").get()));*/
    switch (m_interactionMode) {
        case InteractionMode::MainMenu:
            if (!initializeMenu()) return;
            break;
        case InteractionMode::Game:
            if (!initializeGame(m_playerName)) return;
            break;
    }

    // Initialize save game repository
    if (!m_controller.initializeSaveGameRepository()) {
        throw std::runtime_error(m_controller.getLastError());
    }
    m_fpsCalculator.initialize();
    m_windowSizeChanged(m_WindowSize);
    //HACK: Remove this
    //m_mustCreateNewGame = true;
}

GameWindow::~GameWindow() {
    if (m_mainMenuMode) m_mainMenuMode->unloadGLMapObjects();
    if (m_gameMapMode) m_gameMapMode->unloadGLMapObjects();
    SDL_JoystickClose(m_joystick);
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
    if (m_mustCreateNewGame) {
        createNewGame();
        return;
    }
    if (m_mustReturnToMainMenu) {
        returnToMainMenu();
        return;
    }
    SDL_Event e;
    m_inputDevicesState->processJoystick(m_joystick);
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
            // SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED);
            //HACK: To Remove
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);
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
    m_windowSizeChanged(m_WindowSize);
    return true;
}

bool GameWindow::initializeGame(const std::string &playerName) {
    m_gameMapMode = std::make_unique<GameMapMode>();
    if (!m_gameMapMode->initShaders(m_controller.getResourcesPath())) {
        cerr << m_gameMapMode->getLastError() << "\n";
        return false;
    }
    if (!m_gameMapMode->initialize(m_controller.getResourcesPath(),
            playerName,
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
            if (m_mainMenuMode) m_mainMenuMode->render();
            break;
        case InteractionMode::Game:
            if (m_gameMapMode) m_gameMapMode->render();
            break;
        default:
            break;
    }
    // Display the FPS
    if (m_toggleFPS) {
        m_textService->useShader();
        m_textService->renderText(m_fpsCalculator.getFPSDisplayText(),
                1.0f,                               // X
                static_cast<float>(m_WindowSize.height()) - 24.0f,  // Y
                0.5f,                               // Scale
                glm::vec3(1.0f, 1.0f, 1.0f));       // Color
    }
    SDL_GL_SwapWindow(m_window);
}

void GameWindow::quitRequested() {
    m_mustExit = true;
    Mix_CloseAudio();
}

void GameWindow::newGameRequested(std::string playerName) {
    m_playerName = playerName;
    m_mustCreateNewGame = true;
}

void GameWindow::quitGameRequested() {
    m_mustReturnToMainMenu = true;
}

void GameWindow::createNewGame() {
    m_windowSizeChanged.disconnect_all_slots();
    m_mainMenuMode->quitRequested.disconnect_all_slots();
    m_mainMenuMode->newGameRequested.disconnect_all_slots();
    m_mainMenuMode.reset();
    m_mainMenuMode = nullptr;
    if (initializeGame(m_playerName)) {
        m_interactionMode = InteractionMode::Game;
    } else {
        m_mustExit = true;
        Mix_CloseAudio();
    }
    m_mustCreateNewGame = false;
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
