#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <string>
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "mainMenuCommons.hpp"
#include "mainMenuScreen.hpp"
#include "menuScreenBase.hpp"
#include "size.hpp"

using namespace thewarrior::models;
using namespace thewarrior::ui::components;

namespace thewarrior::ui::screens {

MainMenuScreen::MainMenuScreen(GLContext &glContext)
: MenuScreenBase(glContext),
m_menuWindow(Size<float>(350.0F, 420.0F)),
m_menuButtonNewGame(glContext, Point<float>(0.0F, -135.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonLoadGame(glContext, Point<float>(0.0F, -45.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonSettings(glContext, Point<float>(0.0F, 45.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonQuit(glContext, Point<float>(0.0F, 135.0F), Size<float>(250.0F, 75.0F)) {
}

MainMenuScreen::~MainMenuScreen() {
    unloadTexture(TextureMainMenuLogo);
}

void MainMenuScreen::initialize(const GLComponentBaseInfo &info) {
    MenuScreenBase::initializeBase(info);
    m_menuWindow.initShader(m_shaderProgram);
    m_menuWindow.initialize("", info.texture, info.textService);
    m_menuWindow.setTextureBeginId(29);
    m_menuWindow.setFillCenter(true);
    m_menuButtonNewGame.initialize("New Game", info);
    m_menuButtonLoadGame.initialize("Load Game", info);
    m_menuButtonSettings.initialize("Settings", info);
    m_menuButtonQuit.initialize("Quit", info);
    if (!loadTextures()) {
        throw std::runtime_error(getLastError());
    }
    generateGLElements();
}

void MainMenuScreen::processEvents(SDL_Event &) {
}

void MainMenuScreen::onRender() {
    MenuScreenBase::onRender();
    drawGLObject(TextureMainMenuLogo);
    m_menuWindow.render();
    m_menuButtonNewGame.render();
    m_menuButtonLoadGame.render();
    m_menuButtonSettings.render();
    m_menuButtonQuit.render();
}

void MainMenuScreen::unloadGLMapObjects() {
}

void MainMenuScreen::onGameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    m_menuWindow.gameWindowSizeChanged(size);
    m_menuButtonNewGame.gameWindowSizeChanged(size);
    m_menuButtonLoadGame.gameWindowSizeChanged(size);
    m_menuButtonSettings.gameWindowSizeChanged(size);
    m_menuButtonQuit.gameWindowSizeChanged(size);
}

bool MainMenuScreen::loadTextures() {
    TextureInfo textureMainMenuLogoInfo {
        .name = TextureMainMenuLogo,
        .filename = "mainmenu_logo.png",
        .width = 324,
        .height = 324,
        .tileWidth = 324,
        .tileHeight = 324
    };
    return loadTexture(textureMainMenuLogoInfo);
}

void MainMenuScreen::onGenerateGLElements() {
    MenuScreenBase::onGenerateGLElements();
    generateGLObject(TextureMainMenuLogo,
            std::nullopt,
            HorizontalAlignment::Left,
            VerticalAlignment::Top);
    m_menuWindow.generateGLElements();
    m_menuButtonNewGame.setHasFocus(m_menuSelectedIndex == 0);
    m_menuButtonLoadGame.setHasFocus(m_menuSelectedIndex == 1);
    m_menuButtonSettings.setHasFocus(m_menuSelectedIndex == 2);
    m_menuButtonQuit.setHasFocus(m_menuSelectedIndex == 3);
    m_menuButtonNewGame.generateGLElements();
    m_menuButtonLoadGame.generateGLElements();
    m_menuButtonSettings.generateGLElements();
    m_menuButtonQuit.generateGLElements();
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
