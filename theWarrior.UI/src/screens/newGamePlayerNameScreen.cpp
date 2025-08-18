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
//HACK: Remove this
//m_buttonTest(Point<float>(1.0F, 1.0F), Size<float>(64.0F, 64.0F)),
//m_buttonTest2(Point<float>(100.0F, 1.0F), Size<float>(128.0F, 61.5F)),
//m_buttonTest3(Point<float>(-240.0F, 1.0F), Size<float>(400.0F, 61.5F)) {
}

void NewGamePlayerNameScreen::initialize(const MenuScreenBaseInfo &info) {
    MenuScreenBase::initializeBase(info);
    m_menuWindow.initShader(m_shaderProgram);
    m_menuWindow.initialize("", info.resourcesPath, info.textService);
    m_menuWindow.setTextureBeginId(29);
    m_menuWindow.setFillCenter(true);
    m_onScreenKeyboard.initialize(info.windowGLTexture, m_shaderProgram, info.textService);
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
    //if (m_menuSelectedIndex > 0) {
        //m_menuSelectedIndex--;
        //generateGLElements();
        //playMoveSound();
    //}
}

void NewGamePlayerNameScreen::buttonDownPressed() {
    //if (m_menuSelectedIndex + 1 < 4) {
        //m_menuSelectedIndex++;
        //generateGLElements();
        //playMoveSound();
    //}
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
