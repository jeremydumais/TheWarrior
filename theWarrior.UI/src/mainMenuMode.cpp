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

using namespace thewarrior::models;

namespace thewarrior::ui {

MainMenuMode::MainMenuMode()
: m_glFormService(std::make_shared<GLFormService>()),
m_textures(std::map<std::string, std::shared_ptr<Texture>>()),
m_texturesGL(std::map<std::string, unsigned int>()),
m_menuWindow(Size<float>(350.0F, 420.0F)),
m_menuButtonNewGame(Point<float>(0.0F, -135.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonLoadGame(Point<float>(0.0F, -45.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonSettings(Point<float>(0.0F, 45.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonQuit(Point<float>(0.0F, 135.0F), Size<float>(250.0F, 75.0F)) {
}

void MainMenuMode::initialize(const std::string &resourcesPath,
            std::shared_ptr<GLTextService> textService,
            std::shared_ptr<InputDevicesState> inputDevicesState) {
    m_resourcesPath = resourcesPath;
    m_glFormService->initialize(m_shaderProgram, textService);
    m_textureService.setResourcesPath(resourcesPath);
    m_inputDevicesState = inputDevicesState;
    loadMenuTextures();
    m_menuWindow.initShader(m_shaderProgram);
    m_menuWindow.initialize("", resourcesPath, textService);
    m_menuWindow.setTextureBeginId(29);
    m_menuWindow.setFillCenter(true);
    m_menuButtonNewGame.initialize("New Game", m_windowGLTexture, m_shaderProgram, textService);
    m_menuButtonLoadGame.initialize("Load Game", m_windowGLTexture, m_shaderProgram, textService);
    m_menuButtonSettings.initialize("Settings", m_windowGLTexture, m_shaderProgram, textService);
    m_menuButtonQuit.initialize("Quit", m_windowGLTexture, m_shaderProgram, textService);
    m_menuButtonNewGame.setHasFocus(true);
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
}

void MainMenuMode::update() {
    generateGLElements();
}

void MainMenuMode::render() {
    m_glFormService->drawQuad(m_namedObjects[TextureBackground], m_texturesGL[TextureBackground]);
    m_menuWindow.render();
    m_menuButtonNewGame.render();
    m_menuButtonLoadGame.render();
    m_menuButtonSettings.render();
    m_menuButtonQuit.render();
}

void MainMenuMode::unloadGLMapObjects() {
}

void MainMenuMode::gameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    m_menuWindow.gameWindowSizeChanged(size);
    m_menuButtonNewGame.gameWindowSizeChanged(size);
    m_menuButtonLoadGame.gameWindowSizeChanged(size);
    m_menuButtonSettings.gameWindowSizeChanged(size);
    m_menuButtonQuit.gameWindowSizeChanged(size);
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

void MainMenuMode::generateGLElements() {
    std::vector<GLObject> menuObjects = {};
    m_glFormService->generateQuad(menuObjects,
                                  { 0.0F, 0.0F },
                                  { 1.0F, 1.0F}, m_textures[TextureBackground].get(), 25, m_texturesGL[TextureBackground]);
    m_namedObjects[TextureBackground] = menuObjects.at(0);
    m_menuWindow.generateGLElements();
    m_menuButtonNewGame.generateGLElements();
    m_menuButtonLoadGame.generateGLElements();
    m_menuButtonSettings.generateGLElements();
    m_menuButtonQuit.generateGLElements();
}

}  // namespace thewarrior::ui
