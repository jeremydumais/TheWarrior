#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <fmt/format.h>
#include <cctype>
#include <string>
#include <string_view>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include "glColor.hpp"
#include "glComponentBase.hpp"
#include "newGamePlayerNameScreen.hpp"
#include "glContext.hpp"
#include "mainMenuCommons.hpp"
#include "menuScreenBase.hpp"
#include "point.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::screens {

NewGamePlayerNameScreen::NewGamePlayerNameScreen(GLContext &glContext)
: MenuScreenBase(glContext),
m_onScreenKeyboard(glContext, Point<float>(0.0F, 130.0F)),
m_newGameLabel(glContext, "New Game", Point<float>(0.0F, -300.0F), GLColor::Brown, 0.9F),
m_enterPlayerNameLabel(glContext, "Enter Your Name:", Point<float>(0.0F, -200.0F), GLColor::Brown),
m_playerNameLabel(glContext, "", Point<float>(0.0F, -137.0F), GLColor::Brown, 0.8F),
m_enterNameObject({"Enter name:", {0.0F, 0.0F}, 0.8F}),
m_playerName(""),
m_playerNameLocation(Point<float>(0.0F, 0.0F)),
m_playerNameInitialLocation(Point<float>(-370.0F, -180.0F)),
m_modalDialog(glContext, Point<float>(0.0F, 0.0F), Size<float>(300.0F, 150.0F)) {
    m_onScreenKeyboard.onCharButtonPressed.connect(boost::bind(&NewGamePlayerNameScreen::keyboardCharButtonPressed, this, boost::placeholders::_1));
    m_onScreenKeyboard.onDELButtonPressed.connect(boost::bind(&NewGamePlayerNameScreen::keyboardDELButtonPressed, this));
    m_onScreenKeyboard.onOKButtonPressed.connect(boost::bind(&NewGamePlayerNameScreen::keyboardOKButtonPressed, this));
}

NewGamePlayerNameScreen::~NewGamePlayerNameScreen() {
    unloadTexture(TextureMainMenuTextBox);
}

void NewGamePlayerNameScreen::initialize(const components::GLComponentBaseInfo &info) {
    MenuScreenBase::initializeBase(info);
    m_onScreenKeyboard.initialize(info);
    m_newGameLabel.initialize(info);
    m_enterPlayerNameLabel.initialize(info);
    m_playerNameLabel.initialize(info);
    m_modalDialog.initialize(info);
    if (!loadTextures()) {
        throw std::runtime_error(getLastError());
    }
    generateGLElements();
}

void NewGamePlayerNameScreen::processEvents(SDL_Event &e) {
    if (e.type == SDL_KEYUP) {
        SDL_Keycode key = e.key.keysym.sym;

        if (m_modalDialog.isVisible()) {
            return;
        }
        if (key == SDLK_ESCAPE) {
            playBackSound();
            SDL_Delay(500);
            backPressed();
        }
        if ((key >= SDLK_a && key <= SDLK_z)) {
            int letter = key;
            if (e.key.keysym.mod & (KMOD_SHIFT | KMOD_CAPS)) {
                letter = std::toupper(letter);
            }
            addPlayerNameChar(static_cast<char>(letter));
        } else if (key >= SDLK_0 && key <= SDLK_9 && !(e.key.keysym.mod & (KMOD_SHIFT))) {
            addPlayerNameChar(static_cast<char>(key));
        } else if (key >= SDLK_KP_1 && key <= SDLK_KP_0) {
            switch (key) {
                case SDLK_KP_1:
                    addPlayerNameChar('1');
                    break;
                case SDLK_KP_2:
                    addPlayerNameChar('2');
                    break;
                case SDLK_KP_3:
                    addPlayerNameChar('3');
                    break;
                case SDLK_KP_4:
                    addPlayerNameChar('4');
                    break;
                case SDLK_KP_5:
                    addPlayerNameChar('5');
                    break;
                case SDLK_KP_6:
                    addPlayerNameChar('6');
                    break;
                case SDLK_KP_7:
                    addPlayerNameChar('7');
                    break;
                case SDLK_KP_8:
                    addPlayerNameChar('8');
                    break;
                case SDLK_KP_9:
                    addPlayerNameChar('9');
                    break;
                case SDLK_KP_0:
                    addPlayerNameChar('0');
                    break;
                default:
                    break;
            }
        } else if (key == SDLK_PERIOD ||     // .
                   key == SDLK_KP_PERIOD ||  // .
                   key == SDLK_QUOTE ||      // '
                   key == SDLK_MINUS ||      // -
                   key == SDLK_SPACE) {   // spacebar
            addPlayerNameChar(static_cast<char>(key));
        } else if (key == SDLK_BACKSPACE) {  // backspace
            removePlayerNameChar();
        }
    }
}

void NewGamePlayerNameScreen::update() {
    if (m_modalDialog.isVisible()) {
        m_modalDialog.update();
    } else {
        MenuScreenBase::update();
    }
}

void NewGamePlayerNameScreen::onRender() {
    MenuScreenBase::onRender();
    drawGLObject(TextureMainMenuPanel);
    m_onScreenKeyboard.render();
    drawGLObject(TextureMainMenuTextBox);
    m_newGameLabel.render();
    m_enterPlayerNameLabel.render();
    m_playerNameLabel.render();
    //m_glFormService->drawText(m_enterNameObject);
    m_modalDialog.render();
}

void NewGamePlayerNameScreen::unloadGLMapObjects() {
}

void NewGamePlayerNameScreen::onGameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    MenuScreenBase::onGameWindowSizeChanged(size);
    m_playerNameLocation = {
        (m_screenSize.width() / 2.0F) ,
        (m_screenSize.height() / 2.0F) };
    m_onScreenKeyboard.gameWindowSizeChanged(size);
    m_newGameLabel.gameWindowSizeChanged(size);
    m_enterPlayerNameLabel.gameWindowSizeChanged(size);
    m_playerNameLabel.gameWindowSizeChanged(size);
    m_modalDialog.gameWindowSizeChanged(size);
    generateGLElements();
}

const std::string &NewGamePlayerNameScreen::getPlayerName() const {
    return m_playerName;
}

bool NewGamePlayerNameScreen::loadTextures() {
    TextureInfo textureMainMenuTextBox {
        .name = TextureMainMenuTextBox,
        .filename = "mainmenu_textbox.png",
        .width = 1169,
        .height = 192,
        .tileWidth = 1169,
        .tileHeight = 192
    };
    return loadTexture(textureMainMenuTextBox);
}

void NewGamePlayerNameScreen::onGenerateGLElements() {
    MenuScreenBase::onGenerateGLElements();
    generateGLObject(TextureMainMenuPanel);
    generateGLObject(TextureMainMenuTextBox,
                     Size<>(540, 100),
                     components::HorizontalAlignment::Center,
                     components::VerticalAlignment::Center,
                     Point<int>(0, -130));
    m_onScreenKeyboard.generateGLElements();
    m_newGameLabel.generateGLElements();
    m_enterPlayerNameLabel.generateGLElements();
    m_playerNameLabel.generateGLElements();
    m_modalDialog.generateGLElements();
    m_enterNameObject.position = {m_playerNameLocation.x() + m_playerNameInitialLocation.x(), -m_playerNameLocation.y() + m_playerNameInitialLocation.y() };
    m_enterNameObject.text = fmt::format("Enter your name: {0}", m_playerName);
    m_enterNameObject.color = GLColor::Brown;
}

void NewGamePlayerNameScreen::onButtonUpPressed() {
    m_onScreenKeyboard.buttonUpPress();
}

void NewGamePlayerNameScreen::onButtonDownPressed() {
    m_onScreenKeyboard.buttonDownPress();
}

void NewGamePlayerNameScreen::onButtonLeftPressed() {
    m_onScreenKeyboard.buttonLeftPress();
}

void NewGamePlayerNameScreen::onButtonRightPressed() {
    m_onScreenKeyboard.buttonRightPress();
}

void NewGamePlayerNameScreen::onButtonCancelPressed() {
    if (!m_playerName.empty()) {
        m_onScreenKeyboard.buttonCancelPress();
    } else {
        playBackSound();
        SDL_Delay(500);
        backPressed();
    }
}

void NewGamePlayerNameScreen::onButtonActionPressed() {
    m_onScreenKeyboard.buttonActionPress();
}

bool NewGamePlayerNameScreen::addPlayerNameChar(char c) {
    if (m_playerName.size() < 15) {
        m_playerName.push_back(c);
        m_playerNameLabel.setCaption(m_playerName);
        generateGLElements();
        playClickSound();
        return true;
    }
    playClickDisableSound();
    return false;
}

bool NewGamePlayerNameScreen::removePlayerNameChar() {
    if (!m_playerName.empty()) {
        m_playerName.pop_back();
        m_playerNameLabel.setCaption(m_playerName);
        generateGLElements();
        playClickDisableSound();
        return true;
    }
    playClickDisableSound();
    return false;
}

void NewGamePlayerNameScreen::keyboardCharButtonPressed(char c) {
    addPlayerNameChar(c);
}

void NewGamePlayerNameScreen::keyboardDELButtonPressed() {
    removePlayerNameChar();
}

void NewGamePlayerNameScreen::keyboardOKButtonPressed() {
    bool error = false;
    if (boost::trim_copy(m_playerName).empty()) {
        m_modalDialog.setMessage("The name of the player\ncannot be empty!");
        error = true;
    } else if (!hasAtLeastTwoAlphaAscii(m_playerName)) {
        m_modalDialog.setMessage("The name of the player must\ncontain at least 2 letters!");
        error = true;
    }
    if (error) {
        m_modalDialog.show();
        generateGLElements();
    } else {
        okPressed();
    }
}

bool NewGamePlayerNameScreen::hasAtLeastTwoAlphaAscii(std::string_view value) {
    int count = 0;
    for (char ch : value) {
        if (std::isalpha(ch)) {
            if (++count == 2) return true;
        }
    }
    return false;
}

}  // namespace thewarrior::ui::screens
