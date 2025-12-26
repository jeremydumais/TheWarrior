#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <fmt/format.h>
#include <cctype>
#include <stdexcept>
#include <string>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include "glColor.hpp"
#include "glComponentBase.hpp"
#include "loadGameScreen.hpp"
#include "glContext.hpp"
#include "mainMenuCommons.hpp"
#include "menuScreenBase.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::screens {

LoadGameScreen::LoadGameScreen(GLContext &glContext)
: MenuScreenBase(glContext),
m_gameStateList(glContext, Point<float>(0.0F, 0.0F)),
m_loadGameLabel(glContext, "Load Game", Point<float>(0.0F, -300.0F), GLColor::Brown, 0.9F) {}

void LoadGameScreen::initialize(const components::GLComponentBaseInfo &info) {
    MenuScreenBase::initializeBase(info);
    m_loadGameLabel.initialize(info);
    const auto entries = m_controller.getGameStateList();
    if (!entries.success) {
        throw std::runtime_error(m_controller.getLastError());
    }
    m_gameStateList.initialize(info, entries.gameStateList);
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
    drawGLObject(TextureMainMenuPanel);
    m_loadGameLabel.render();
    m_gameStateList.render();
}

void LoadGameScreen::unloadGLMapObjects() {
}

void LoadGameScreen::onGameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    MenuScreenBase::onGameWindowSizeChanged(size);
    m_gameStateList.gameWindowSizeChanged(size);
    m_loadGameLabel.gameWindowSizeChanged(size);
    generateGLElements();
}

bool LoadGameScreen::loadTextures() {
    return true;
}

void LoadGameScreen::onGenerateGLElements() {
    MenuScreenBase::onGenerateGLElements();
    generateGLObject(TextureMainMenuPanel);
    m_gameStateList.generateGLElements();
    m_loadGameLabel.generateGLElements();
}

void LoadGameScreen::onButtonUpPressed() {
    m_gameStateList.buttonUpPress();
}

void LoadGameScreen::onButtonDownPressed() {
    m_gameStateList.buttonDownPress();
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
