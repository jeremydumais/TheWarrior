#include <fmt/format.h>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "mainMenuMode.hpp"
#include "glTexture.hpp"
#include "size.hpp"
#include "texture.hpp"
#include "textureInfo.hpp"
#include "screens/mainMenuCommons.hpp"

using namespace thewarrior::models;
using namespace thewarrior::ui::screens;

namespace thewarrior::ui {

MainMenuMode::MainMenuMode()
: m_glFormService(std::make_shared<GLFormService>()),
m_textures(std::map<std::string, std::shared_ptr<Texture>>()),
m_texturesGL(std::map<std::string, unsigned int>()),
m_mainScreen(m_textures, m_texturesGL) {
}

void MainMenuMode::initialize(const std::string &resourcesPath,
            std::shared_ptr<GLTextService> textService,
            std::shared_ptr<InputDevicesState> inputDevicesState) {
    m_resourcesPath = resourcesPath;
    m_glFormService->initialize(m_shaderProgram, textService);
    m_textureService.setResourcesPath(resourcesPath);
    m_inputDevicesState = inputDevicesState;
    loadMenuTextures();
    m_mainScreen.initialize(resourcesPath, m_shaderProgram, textService, inputDevicesState, m_windowGLTexture);
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
            break;
        case MainMenuInputMode::LoadGame:
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
            break;
        case MainMenuInputMode::LoadGame:
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
            break;
        case MainMenuInputMode::LoadGame:
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
    m_mainScreen.gameWindowSizeChanged(size);
}

void MainMenuMode::loadMenuTextures() {
    // Clear existing textures in graphics memory
    for (auto &glTexture : m_texturesGL) {
        m_textureService.unloadTexture(glTexture.second);
    }
    m_texturesGL.clear();
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
        m_textureService.loadTexture(*m_textures[TextureBackground], m_texturesGL[TextureBackground]);
    } catch (const std::invalid_argument &err) {
        std::cerr << "Unable to load the main menu background texture: " << err.what() << std::endl;
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
        m_textureService.loadTexture(*m_windowGLTexture);
    } catch (const std::invalid_argument &err) {
        std::cerr << "Unable to load the window texture: " << err.what() << std::endl;
    }
}

}  // namespace thewarrior::ui
