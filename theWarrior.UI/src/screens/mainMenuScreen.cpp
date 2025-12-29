#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <fmt/format.h>
#include <optional>
#include <string>
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glPanel.hpp"
#include "mainMenuCommons.hpp"
#include "mainMenuScreen.hpp"
#include "menuScreenBase.hpp"
#include "size.hpp"

using namespace thewarrior::models;
using namespace thewarrior::ui::components;

namespace thewarrior::ui::screens {

MainMenuScreen::MainMenuScreen(GLContext &glContext)
: MenuScreenBase(glContext),
m_menuButtonNewGame(glContext, "New Game", Point<float>(0.0F, -135.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonLoadGame(glContext, "Load Game", Point<float>(0.0F, -45.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonSettings(glContext, "Settings", Point<float>(0.0F, 45.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonQuit(glContext, "Quit", Point<float>(0.0F, 135.0F), Size<float>(250.0F, 75.0F)),
m_menuPanel(glContext, Size<float>(350.0F, 420.0F)) {
    registerComponent(&m_menuButtonNewGame);
    registerComponent(&m_menuButtonLoadGame);
    registerComponent(&m_menuButtonSettings);
    registerComponent(&m_menuButtonQuit);
    registerComponent(&m_menuPanel);
}

MainMenuScreen::~MainMenuScreen() {
    unloadTexture(TextureMainMenuLogo);
}

void MainMenuScreen::onInitialize(const GLComponentBaseInfo &) {
    if (!loadTextures()) {
        throw std::runtime_error(getLastError());
    }
}

void MainMenuScreen::processEvents(SDL_Event &) {
}

void MainMenuScreen::onRender() {
    MenuScreenBase::onRender();
    drawGLObject(TextureMainMenuLogo);
    m_menuPanel.render();
    m_menuButtonNewGame.render();
    m_menuButtonLoadGame.render();
    m_menuButtonSettings.render();
    m_menuButtonQuit.render();
}

bool MainMenuScreen::loadTextures() {
    return loadTexture(TextureMainMenuLogo, "mainmenu_logo.png", 324, 324);
}

void MainMenuScreen::onGenerateGLElements() {
    MenuScreenBase::onGenerateGLElements();
    generateGLObject(TextureMainMenuLogo,
            std::nullopt,
            HorizontalAlignment::Left,
            VerticalAlignment::Top);
    m_menuButtonNewGame.setHasFocus(m_menuSelectedIndex == 0);
    m_menuButtonLoadGame.setHasFocus(m_menuSelectedIndex == 1);
    m_menuButtonSettings.setHasFocus(m_menuSelectedIndex == 2);
    m_menuButtonQuit.setHasFocus(m_menuSelectedIndex == 3);
}

void MainMenuScreen::onButtonUpPressed() {
    if (m_menuSelectedIndex > 0) {
        m_menuSelectedIndex--;
        generateGLElements();
        playMoveSound();
    }
}

void MainMenuScreen::onButtonDownPressed() {
    if (m_menuSelectedIndex + 1 < 4) {
        m_menuSelectedIndex++;
        generateGLElements();
        playMoveSound();
    }
}

void MainMenuScreen::onButtonCancelPressed() {
}

void MainMenuScreen::onButtonActionPressed() {
    if (m_menuSelectedIndex == 0) {
        playClickSound();
        SDL_Delay(500);
        newGamePressed();
    } else if (m_menuSelectedIndex == 1) {
        playClickSound();
        SDL_Delay(500);
        loadGamePressed();
    } else if (m_menuSelectedIndex == 3) {
        playClickSound();
        SDL_Delay(500);
        quitPressed();
    }
}

}  // namespace thewarrior::ui::screens
