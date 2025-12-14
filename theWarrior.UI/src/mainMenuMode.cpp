#include <SDL2/SDL_mixer.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include "mainMenuMode.hpp"
#include "glComponentBase.hpp"
#include "glTexture.hpp"
#include "texture.hpp"
#include "textureInfo.hpp"
#include "mainMenuCommons.hpp"

using namespace thewarrior::models;
using namespace thewarrior::ui::screens;

namespace thewarrior::ui {

MainMenuMode::MainMenuMode()
: m_textureService(std::make_shared<GLTextureService>()),
m_glFormService(std::make_shared<GLFormService>()),
m_textures(std::map<std::string, std::shared_ptr<Texture>>()),
m_texturesGL(std::map<std::string, unsigned int>()),
m_glContext(m_textures, m_texturesGL),
m_mainScreen(m_glContext),
m_loadGameScreen(m_glContext),
m_newGamePlayerNameScreen(m_glContext) {
}

MainMenuMode::~MainMenuMode() {
    unloadMenuTextures();
    Mix_HaltMusic();
}

void MainMenuMode::initialize(const std::string &resourcesPath,
            std::shared_ptr<GLTextService> textService,
            std::shared_ptr<InputDevicesState> inputDevicesState) {
    m_resourcesPath = resourcesPath;
    m_glFormService->initialize(m_shaderProgram, textService);
    m_textureService->setResourcesPath(resourcesPath);
    m_inputDevicesState = inputDevicesState;
    loadMenuTextures();
    loadMenuSounds();
    components::GLComponentBaseInfo baseInfo {
        .resourcesPath = resourcesPath,
        .shaderProgram = m_shaderProgram,
        .textService = textService,
        .textureService = m_textureService,
        .inputDevicesState = inputDevicesState,
        .texture = m_windowGLTexture,
        .menuMoveSound = m_menuMoveSound,
        .menuClickSound = m_menuClickSound,
        .menuClickDisableSound = m_menuClickDisableSound,
        .menuBackSound = m_menuBackSound
    };
    m_mainScreen.initialize(baseInfo);
    if (!m_mainScreen.loadTextures()) {
        throw std::runtime_error(m_mainScreen.getLastError());
    }
    m_mainScreen.newGamePressed.connect(boost::bind(&MainMenuMode::newGamePressed, this));
    m_mainScreen.loadGamePressed.connect(boost::bind(&MainMenuMode::loadGamePressed, this));
    m_mainScreen.quitPressed.connect(boost::bind(&MainMenuMode::quitPressed, this));
    m_newGamePlayerNameScreen.initialize(baseInfo);
    m_newGamePlayerNameScreen.backPressed.connect(boost::bind(&MainMenuMode::backToMainMenu, this));
    m_newGamePlayerNameScreen.okPressed.connect(boost::bind(&MainMenuMode::newGameConfirmed, this));
    m_loadGameScreen.initialize(baseInfo);
    m_loadGameScreen.backPressed.connect(boost::bind(&MainMenuMode::backToMainMenu, this));
    m_loadGameScreen.okPressed.connect(boost::bind(&MainMenuMode::loadGameConfirmed, this));
    Mix_PlayMusic(m_backgroundMusic, -1);  // loop forever
}

bool MainMenuMode::initShaders(const std::string &resourcesPath) {
    m_shaderProgram = std::make_shared<GLShaderProgram>(fmt::format("{0}/shaders/window_330_vs.glsl", resourcesPath),
            fmt::format("{0}/shaders/window_330_fs.glsl", resourcesPath));
    if (!m_shaderProgram->compileShaders()) {
        m_lastError = m_shaderProgram->getLastError();
        return false;
    }
    if (!m_shaderProgram->linkShaders({ "vertex" })) {
        m_lastError = m_shaderProgram->getLastError();
        return false;
    }
    return true;
}

const std::string& MainMenuMode::getLastError() const {
    return m_lastError;
}

void MainMenuMode::processEvents(SDL_Event &e) {
    switch (m_inputMode) {
        case MainMenuInputMode::Main:
            m_mainScreen.processEvents(e);
            break;
        case MainMenuInputMode::NewGamePlayerName:
            m_newGamePlayerNameScreen.processEvents(e);
            break;
        case MainMenuInputMode::LoadGame:
            m_loadGameScreen.processEvents(e);
            break;
        case MainMenuInputMode::Settings:
            break;
        default:
            break;
    }
}

void MainMenuMode::update() {
    switch (m_inputMode) {
        case MainMenuInputMode::Main:
            m_mainScreen.update();
            break;
        case MainMenuInputMode::NewGamePlayerName:
            m_newGamePlayerNameScreen.update();
            break;
        case MainMenuInputMode::LoadGame:
            m_loadGameScreen.update();
            break;
        case MainMenuInputMode::Settings:
            break;
        default:
            break;
    }
}

void MainMenuMode::render() {
    switch (m_inputMode) {
        case MainMenuInputMode::Main:
            m_mainScreen.render();
            break;
        case MainMenuInputMode::NewGamePlayerName:
            m_newGamePlayerNameScreen.render();
            break;
        case MainMenuInputMode::LoadGame:
            m_loadGameScreen.render();
            break;
        case MainMenuInputMode::Settings:
            break;
        default:
            break;
    }
}

void MainMenuMode::unloadGLMapObjects() {
}

void MainMenuMode::gameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    m_glFormService->gameWindowSizeChanged(size);
    m_mainScreen.gameWindowSizeChanged(size);
    m_newGamePlayerNameScreen.gameWindowSizeChanged(size);
    m_loadGameScreen.gameWindowSizeChanged(size);
}

void MainMenuMode::loadMenuTextures() {
    unloadMenuTextures();
    // Background
    TextureInfo textureBackgroundInfo {
        .name = "background",
        .filename = "mainmenu_background.png",
        .width = 1536,
        .height = 1024,
        .tileWidth = 1536,
        .tileHeight = 1024
    };
    try {
        m_textures[TextureBackground] = std::make_shared<Texture>(textureBackgroundInfo);
        m_textureService->loadTexture(*m_textures[TextureBackground], m_texturesGL[TextureBackground]);
    } catch (const std::invalid_argument &err) {
        std::cerr << "Unable to load the main menu background texture: " << err.what() << std::endl;
    }
    // Menu Panel
    TextureInfo textureMenuPanelInfo {
        .name = "menupanel",
        .filename = "mainmenu_panel.png",
        .width = 1100,
        .height = 777,
        .tileWidth = 1100,
        .tileHeight = 777
    };
    try {
        m_textures[TextureMainMenuPanel] = std::make_shared<Texture>(textureMenuPanelInfo);
        m_textureService->loadTexture(*m_textures[TextureMainMenuPanel], m_texturesGL[TextureMainMenuPanel]);
    } catch (const std::invalid_argument &err) {
        std::cerr << "Unable to load the main menu panel texture: " << err.what() << std::endl;
    }
    // Button
    TextureInfo textureInfoWindow {
        .name = "window",
        .filename = "window.png",
        .width = 256,
        .height = 256,
        .tileWidth = 32,
        .tileHeight = 32
    };
    try {
        m_textures[TextureWindow] = std::make_shared<Texture>(textureInfoWindow);
        m_windowGLTexture = std::make_shared<GLTexture>(*m_textures[TextureWindow], 0);
        m_textureService->loadTexture(*m_windowGLTexture);
    } catch (const std::invalid_argument &err) {
        std::cerr << "Unable to load the window texture: " << err.what() << std::endl;
    }
}

void MainMenuMode::unloadMenuTextures() {
    // Clear existing textures in graphics memory
    for (auto &glTexture : m_texturesGL) {
        m_textureService->unloadTexture(glTexture.second);
    }
    m_texturesGL.clear();
}

void MainMenuMode::loadMenuSounds() {
    m_backgroundMusic = Mix_LoadMUS(fmt::format("{0}/sounds/main_menu.mp3", m_resourcesPath).c_str());
    if (!m_backgroundMusic) {
        std::cerr << fmt::format("Mix_LoadMUS error: {0}", Mix_GetError()) << std::endl;
        return;
    }
    m_menuMoveSound = std::shared_ptr<Mix_Chunk>(Mix_LoadWAV(fmt::format("{0}/sounds/menu_move.wav", m_resourcesPath).c_str()), Mix_FreeChunk);
    if (!m_menuMoveSound) {
        std::cerr << fmt::format("Mix_LoadMUS error: {0}", Mix_GetError()) << std::endl;
        return;
    }
    m_menuClickSound = std::shared_ptr<Mix_Chunk>(Mix_LoadWAV(fmt::format("{0}/sounds/menu_click.wav", m_resourcesPath).c_str()), Mix_FreeChunk);
    if (!m_menuClickSound) {
        std::cerr << fmt::format("Mix_LoadMUS error: {0}", Mix_GetError()) << std::endl;
        return;
    }
    m_menuClickDisableSound = std::shared_ptr<Mix_Chunk>(Mix_LoadWAV(fmt::format("{0}/sounds/menu_click_disable.wav", m_resourcesPath).c_str()), Mix_FreeChunk);
    if (!m_menuClickDisableSound) {
        std::cerr << fmt::format("Mix_LoadMUS error: {0}", Mix_GetError()) << std::endl;
        return;
    }
    m_menuBackSound = std::shared_ptr<Mix_Chunk>(Mix_LoadWAV(fmt::format("{0}/sounds/menu_back.wav", m_resourcesPath).c_str()), Mix_FreeChunk);
    if (!m_menuBackSound) {
        std::cerr << fmt::format("Mix_LoadMUS error: {0}", Mix_GetError()) << std::endl;
        return;
    }
}

void MainMenuMode::newGamePressed() {
    m_inputMode = MainMenuInputMode::NewGamePlayerName;
}

void MainMenuMode::loadGamePressed() {
    m_inputMode = MainMenuInputMode::LoadGame;
}

void MainMenuMode::quitPressed() {
    quitRequested();
    Mix_FreeMusic(m_backgroundMusic);
}

void MainMenuMode::backToMainMenu() {
    m_inputMode = MainMenuInputMode::Main;
}

void MainMenuMode::newGameConfirmed() {
    newGameRequested(m_newGamePlayerNameScreen.getPlayerName());
}

void MainMenuMode::loadGameConfirmed() {
}

}  // namespace thewarrior::ui
