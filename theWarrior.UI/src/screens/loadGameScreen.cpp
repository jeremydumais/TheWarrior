#include <SDL2/SDL_events.h>
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
#include "point.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::screens {

LoadGameScreen::LoadGameScreen(GLContext &glContext)
: MenuScreenBase(glContext),
m_gameStateList(glContext, Point<float>(0.0F, 0.0F)),
m_loadGameLabel(glContext, "Load Game", Point<float>(0.0F, -300.0F), GLColor::Brown, 0.9F),
m_noSavedGameLabel(glContext, "No saved adventures found", Point<float>(0.0F, -100.0F), GLColor::Gray, 0.7F),
m_noSavedGameInstructionsLabel1(glContext, "Your journey has yet to begin.", Point<float>(0.0F, -30.0F), GLColor::Gray, 0.46F),
m_noSavedGameInstructionsLabel2(glContext, "Start a new game to create your first save.", Point<float>(0.0F, 40.0F), GLColor::Gray, 0.46F),
m_menuButtonLoad(glContext, "Load", Point<float>(270.0F, 300.0F), Size<float>(250.0F, 75.0F)),
m_gameStates({}),
m_focusElement(FocusElement::GameStateList) {
    registerComponent(&m_gameStateList);
    registerComponent(&m_loadGameLabel);
    registerComponent(&m_menuButtonLoad);
    registerComponent(&m_noSavedGameLabel);
    registerComponent(&m_noSavedGameInstructionsLabel1);
    registerComponent(&m_noSavedGameInstructionsLabel2);
    m_gameStateList.onOKButtonPressed.connect(boost::bind(&LoadGameScreen::gameStateListOKButtonPressed, this));
    m_gameStateList.onCancelButtonPressed.connect(boost::bind(&LoadGameScreen::gameStateListCancelButtonPressed, this));
}

void LoadGameScreen::onInitialize(const components::GLComponentBaseInfo &) {
    m_menuButtonLoad.setEnabled(false);
    const auto entries = m_controller.getGameStateList();
    if (!entries.success) {
        throw std::runtime_error(m_controller.getLastError());
    }
    m_gameStates = entries.gameStateList;
    m_gameStateList.setGameStates(m_gameStates);
    if (!loadTextures()) {
        throw std::runtime_error(getLastError());
    }
}

bool LoadGameScreen::loadTextures() {
    return loadTexture(TextureMainMenuButtonOkEnabled, "mainmenu_buttonok_enabled.png", 362, 100) &&
        loadTexture(TextureMainMenuButtonOkDisabled, "mainmenu_buttonok_disabled.png", 370, 97);
}

void LoadGameScreen::update() {
    if (m_focusElement == FocusElement::GameStateList) {
        m_gameStateList.update();
    } else {
        MenuScreenBase::update();
    }
}

void LoadGameScreen::onRender() {
    MenuScreenBase::onRender();
    drawGLObject(TextureMainMenuPanel);
    m_loadGameLabel.render();
    if (!m_gameStates.empty()) {
        m_gameStateList.render();
        m_menuButtonLoad.render();
    } else {
        m_noSavedGameLabel.render();
        m_noSavedGameInstructionsLabel1.render();
        m_noSavedGameInstructionsLabel2.render();
    }
}

void LoadGameScreen::reset() {
    m_focusElement = FocusElement::GameStateList;
    m_menuButtonLoad.setEnabled(false);
    m_menuButtonLoad.setHasFocus(false);
    m_gameStateList.reset();
}

const std::string &LoadGameScreen::getSelectedGameStateFileName() const {
    return m_gameStateList.getSelectedGameState().fileName;
}

void LoadGameScreen::onGenerateGLElements() {
    MenuScreenBase::onGenerateGLElements();
    generateGLObject(TextureMainMenuPanel);
}

void LoadGameScreen::onButtonCancelPressed() {
    playBackSound();
    SDL_Delay(500);
    m_focusElement = FocusElement::GameStateList;
    m_menuButtonLoad.setEnabled(false);
    m_menuButtonLoad.setHasFocus(false);
}

void LoadGameScreen::onButtonActionPressed() {
    okPressed();
}

void LoadGameScreen::gameStateListOKButtonPressed() {
    m_focusElement = FocusElement::LoadButton;
    m_menuButtonLoad.setEnabled(true);
    m_menuButtonLoad.setHasFocus(true);
}

void LoadGameScreen::gameStateListCancelButtonPressed() {
    playBackSound();
    SDL_Delay(500);
    backPressed();
}

}  // namespace thewarrior::ui::screens
