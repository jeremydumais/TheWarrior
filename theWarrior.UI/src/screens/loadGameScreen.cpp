#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <fmt/format.h>
#include <cctype>
#include <map>
#include <memory>
#include <string>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include "glComponentBase.hpp"
#include "loadGameScreen.hpp"
#include "mainMenuCommons.hpp"
#include "menuScreenBase.hpp"
#include "size.hpp"
#include "texture.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::screens {

LoadGameScreen::LoadGameScreen(std::map<std::string, std::shared_ptr<Texture>> &textures,
                               std::map<std::string, unsigned int> &texturesGL)
: MenuScreenBase(textures, texturesGL),
m_menuWindow(Size<float>(850.0F, 620.0F)) {}

void LoadGameScreen::initialize(const components::GLComponentBaseInfo &info) {
    MenuScreenBase::initializeBase(info);
    m_menuWindow.initShader(m_shaderProgram);
    m_menuWindow.initialize("", info.texture, info.textService);
    m_menuWindow.setTextureBeginId(29);
    m_menuWindow.setFillCenter(true);

    generateGLElements();
}

void LoadGameScreen::processEvents(SDL_Event &e) {
    if (e.type == SDL_KEYUP) {
        SDL_Keycode key = e.key.keysym.sym;

        if (key == SDLK_ESCAPE) {
            playBackSound();
            SDL_Delay(500);
            backPressed();
        }
    }
}

void LoadGameScreen::update() {
    MenuScreenBase::update();
}

void LoadGameScreen::onRender() {
    MenuScreenBase::onRender();
    m_glFormService->drawQuad(m_namedObjects[TextureMainMenuLogo], m_texturesGL[TextureMainMenuLogo]);
    m_menuWindow.render();
}

void LoadGameScreen::unloadGLMapObjects() {
}

void LoadGameScreen::onGameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    MenuScreenBase::onGameWindowSizeChanged(size);
    m_menuWindow.gameWindowSizeChanged(size);
    generateGLElements();
}

bool LoadGameScreen::loadTextures() {
    return true;
}

void LoadGameScreen::onGenerateGLElements() {
    MenuScreenBase::onGenerateGLElements();
    m_menuWindow.generateGLElements();
}

void LoadGameScreen::onButtonUpPressed() {
}

void LoadGameScreen::onButtonDownPressed() {
}

void LoadGameScreen::onButtonLeftPressed() {
}

void LoadGameScreen::onButtonRightPressed() {
}

void LoadGameScreen::onButtonCancelPressed() {
    playBackSound();
    SDL_Delay(500);
    backPressed();
}

void LoadGameScreen::onButtonActionPressed() {
}

void LoadGameScreen::keyboardOKButtonPressed() {
    okPressed();
}

}  // namespace thewarrior::ui::screens
