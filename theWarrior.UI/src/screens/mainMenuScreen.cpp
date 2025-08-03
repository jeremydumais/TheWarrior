#include <fmt/format.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "mainMenuScreen.hpp"
#include "glTexture.hpp"
#include "size.hpp"
#include "texture.hpp"
#include "screens/mainMenuCommons.hpp"

using namespace thewarrior::models;
using namespace thewarrior::ui::components;

namespace thewarrior::ui::screens {

MainMenuScreen::MainMenuScreen(std::map<std::string, std::shared_ptr<Texture>> &textures,
                               std::map<std::string, unsigned int> &texturesGL)
: m_glFormService(std::make_shared<GLFormService>()),
m_textures(textures),
m_texturesGL(texturesGL),
m_menuWindow(Size<float>(350.0F, 420.0F)),
m_menuButtonNewGame(Point<float>(0.0F, -135.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonLoadGame(Point<float>(0.0F, -45.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonSettings(Point<float>(0.0F, 45.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonQuit(Point<float>(0.0F, 135.0F), Size<float>(250.0F, 75.0F)) {
}

void MainMenuScreen::initialize(const std::string &resourcesPath,
            std::shared_ptr<GLShaderProgram> shaderProgram,
            std::shared_ptr<GLTextService> textService,
            std::shared_ptr<InputDevicesState> inputDevicesState,
            std::shared_ptr<GLTexture> windowGLTexture) {
    m_resourcesPath = resourcesPath;
    m_shaderProgram = shaderProgram;
    m_glFormService->initialize(m_shaderProgram, textService);
    m_textureService.setResourcesPath(resourcesPath);
    m_inputDevicesState = inputDevicesState;
    m_windowGLTexture = windowGLTexture;
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

const std::string& MainMenuScreen::getLastError() const {
    return m_lastError;
}

void MainMenuScreen::processEvents(SDL_Event &e) {
}

void MainMenuScreen::update() {
    generateGLElements();
}

void MainMenuScreen::render() {
    m_glFormService->drawQuad(m_namedObjects[TextureBackground], m_texturesGL[TextureBackground]);
    m_menuWindow.render();
    m_menuButtonNewGame.render();
    m_menuButtonLoadGame.render();
    m_menuButtonSettings.render();
    m_menuButtonQuit.render();
}

void MainMenuScreen::unloadGLMapObjects() {
}

void MainMenuScreen::gameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    m_menuWindow.gameWindowSizeChanged(size);
    m_menuButtonNewGame.gameWindowSizeChanged(size);
    m_menuButtonLoadGame.gameWindowSizeChanged(size);
    m_menuButtonSettings.gameWindowSizeChanged(size);
    m_menuButtonQuit.gameWindowSizeChanged(size);
}

void MainMenuScreen::generateGLElements() {
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

}  // namespace thewarrior::ui::screens
