#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <fmt/format.h>
#include <map>
#include <memory>
#include <string>
#include "mainMenuCommons.hpp"
#include "newGamePlayerNameScreen.hpp"
#include "menuScreenBase.hpp"
#include "point.hpp"
#include "size.hpp"
#include "texture.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::screens {

NewGamePlayerNameScreen::NewGamePlayerNameScreen(std::map<std::string, std::shared_ptr<Texture>> &textures,
                               std::map<std::string, unsigned int> &texturesGL)
: MenuScreenBase(textures, texturesGL),
m_menuWindow(Size<float>(850.0F, 620.0F)),
m_onScreenKeyboard(Point<float>(1.0F, 1.0F)) {
}

void NewGamePlayerNameScreen::initialize(const MenuScreenBaseInfo &info) {
    MenuScreenBase::initializeBase(info);
    m_menuWindow.initShader(m_shaderProgram);
    m_menuWindow.initialize("", info.resourcesPath, info.textService);
    m_menuWindow.setTextureBeginId(29);
    m_menuWindow.setFillCenter(true);
    m_onScreenKeyboard.initialize(info.windowGLTexture,
                                  m_shaderProgram,
                                  info.textService,
                                  info.menuMoveSound,
                                  info.menuClickSound);
}

void NewGamePlayerNameScreen::processEvents(SDL_Event &) {
}

void NewGamePlayerNameScreen::update() {
    MenuScreenBase::updateBase();
    generateGLElements();
}

void NewGamePlayerNameScreen::render() {
    MenuScreenBase::renderBase();
    m_glFormService->drawQuad(m_namedObjects[TextureMainMenuLogo], m_texturesGL[TextureMainMenuLogo]);
    m_menuWindow.render();
    m_onScreenKeyboard.render();
}

void NewGamePlayerNameScreen::unloadGLMapObjects() {
}

void NewGamePlayerNameScreen::gameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    MenuScreenBase::gameWindowSizeChangedBase(size);
    m_menuWindow.gameWindowSizeChanged(size);
    m_onScreenKeyboard.gameWindowSizeChanged(size);
}

bool NewGamePlayerNameScreen::loadTextures() {
    return true;
}

void NewGamePlayerNameScreen::generateGLElements() {
    MenuScreenBase::generateGLElementsBase();
    m_menuWindow.generateGLElements();
    m_onScreenKeyboard.generateGLElements();
}

void NewGamePlayerNameScreen::buttonUpPressed() {
    m_onScreenKeyboard.buttonUpPress();
}

void NewGamePlayerNameScreen::buttonDownPressed() {
    m_onScreenKeyboard.buttonDownPress();
}

void NewGamePlayerNameScreen::buttonLeftPressed() {
    m_onScreenKeyboard.buttonLeftPress();
}

void NewGamePlayerNameScreen::buttonRightPressed() {
    m_onScreenKeyboard.buttonRightPress();
}

void NewGamePlayerNameScreen::buttonCancelPressed() {
    playClickSound();
    SDL_Delay(500);
    backPressed();
}

void NewGamePlayerNameScreen::buttonActionPressed() {
    if (m_menuSelectedIndex == 0) {
        playClickSound();
    } else if (m_menuSelectedIndex == 3) {
        playClickSound();
    }
}

}  // namespace thewarrior::ui::screens
