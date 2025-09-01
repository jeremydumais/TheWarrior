#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <fmt/format.h>
#include <cctype>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <boost/algorithm/string/case_conv.hpp>
#include "glComponentBase.hpp"
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
m_textService(nullptr),
m_menuWindow(Size<float>(850.0F, 620.0F)),
m_onScreenKeyboard(Point<float>(1.0F, 1.0F)),
m_enterNameObject({"Enter name:", {0.0F, 0.0F}, 0.8F}),
m_playerName(""),
m_playerNameLocation(Point<float>(0.0F, 0.0F)),
m_playerNameInitialLocation(Point<float>(-300.0F, -230.0F)),
m_modalDialog(Point<float>(0.0F, 0.0F), Size<float>(300.0F, 150.0F)) {
    m_onScreenKeyboard.onCharButtonPressed.connect(boost::bind(&NewGamePlayerNameScreen::keyboardCharButtonPressed, this, boost::placeholders::_1));
    m_onScreenKeyboard.onDELButtonPressed.connect(boost::bind(&NewGamePlayerNameScreen::keyboardDELButtonPressed, this));
    m_onScreenKeyboard.onOKButtonPressed.connect(boost::bind(&NewGamePlayerNameScreen::keyboardOKButtonPressed, this));
}

void NewGamePlayerNameScreen::initialize(const MenuScreenBaseInfo &info) {
    MenuScreenBase::initializeBase(info);
    m_textService = info.textService;
    components::GLComponentBaseInfo componentInfo {
        .shaderProgram = info.shaderProgram,
        .textService = info.textService,
        .inputDevicesState = info.inputDevicesState,
        .texture = info.windowGLTexture,
        .menuMoveSound = info.menuMoveSound,
        .menuClickSound = info.menuClickSound,
        .menuClickDisableSound = info.menuClickDisableSound,
        .menuBackSound = info.menuBackSound
    };
    m_menuWindow.initShader(m_shaderProgram);
    m_menuWindow.initialize("", info.windowGLTexture, info.textService);
    m_menuWindow.setTextureBeginId(29);
    m_menuWindow.setFillCenter(true);

    m_onScreenKeyboard.initialize(componentInfo);
    m_modalDialog.initialize(componentInfo);
    //m_modalDialog.setMessage("The name of the player\ncannot be empty!");
    //m_modalDialog.setMessage("The name of the player\ncannot be white spaces!");
    //m_modalDialog.setMessage("The name of the player must\ncontain at least 2 letters!");
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
        MenuScreenBase::updateBase();
    }
}

void NewGamePlayerNameScreen::render() {
    MenuScreenBase::renderBase();
    m_glFormService->drawQuad(m_namedObjects[TextureMainMenuLogo], m_texturesGL[TextureMainMenuLogo]);
    m_menuWindow.render();
    m_onScreenKeyboard.render();
    m_glFormService->drawText(m_enterNameObject);
    m_modalDialog.render();
}

void NewGamePlayerNameScreen::unloadGLMapObjects() {
}

void NewGamePlayerNameScreen::gameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    MenuScreenBase::gameWindowSizeChangedBase(size);
    m_playerNameLocation = {
        (m_screenSize.width() / 2.0F) ,
        (m_screenSize.height() / 2.0F) };
    m_menuWindow.gameWindowSizeChanged(size);
    m_onScreenKeyboard.gameWindowSizeChanged(size);
    m_modalDialog.gameWindowSizeChanged(size);
    generateGLElements();
}

bool NewGamePlayerNameScreen::loadTextures() {
    return true;
}

void NewGamePlayerNameScreen::generateGLElements() {
    MenuScreenBase::generateGLElementsBase();
    m_menuWindow.generateGLElements();
    m_onScreenKeyboard.generateGLElements();
    m_modalDialog.generateGLElements();
    m_enterNameObject.position = {m_playerNameLocation.x() + m_playerNameInitialLocation.x(), -m_playerNameLocation.y() + m_playerNameInitialLocation.y() };
    m_enterNameObject.text = fmt::format("Enter name: {0}", m_playerName);
    m_enterNameObject.color = GLColor::Gray;
}

void NewGamePlayerNameScreen::buttonUpPressed() {
    if (!m_modalDialog.isVisible()) {
        m_onScreenKeyboard.buttonUpPress();
    }
}

void NewGamePlayerNameScreen::buttonDownPressed() {
    if (!m_modalDialog.isVisible()) {
        m_onScreenKeyboard.buttonDownPress();
    }
}

void NewGamePlayerNameScreen::buttonLeftPressed() {
    if (!m_modalDialog.isVisible()) {
        m_onScreenKeyboard.buttonLeftPress();
    }
}

void NewGamePlayerNameScreen::buttonRightPressed() {
    if (!m_modalDialog.isVisible()) {
        m_onScreenKeyboard.buttonRightPress();
    }
}

void NewGamePlayerNameScreen::buttonCancelPressed() {
    if (m_modalDialog.isVisible()) {
        m_modalDialog.hide();
        playBackSound();
    } else {
        if (!m_playerName.empty()) {
            m_onScreenKeyboard.buttonCancelPress();
        } else {
            playBackSound();
            SDL_Delay(500);
            backPressed();
        }
    }
}

void NewGamePlayerNameScreen::buttonActionPressed() {
    if (m_modalDialog.isVisible()) {
        //TODO: Handle the button click on the dialog
        m_modalDialog.hide();
        playClickSound();
    } else {
        m_onScreenKeyboard.buttonActionPress();
    }
}

bool NewGamePlayerNameScreen::addPlayerNameChar(char c) {
    if (m_playerName.size() < 15) {
        m_playerName.push_back(c);
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
    //TODO: Validate the player name
    m_modalDialog.setMessage("Validation to be done");
    m_modalDialog.show();
    generateGLElements();
}

}  // namespace thewarrior::ui::screens
