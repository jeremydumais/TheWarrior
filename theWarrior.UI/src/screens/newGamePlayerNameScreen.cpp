#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <fmt/format.h>
#include <cctype>
#include <map>
#include <memory>
#include <string>
#include <boost/algorithm/string/case_conv.hpp>
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
m_location(Point<float>(0.0F, 0.0F)),
m_initialLocation(Point<float>(-300.0F, -230.0F)) {
    m_onScreenKeyboard.onCharButtonPressed.connect(boost::bind(&NewGamePlayerNameScreen::keyboardCharButtonPressed, this, boost::placeholders::_1));
    m_onScreenKeyboard.onDELButtonPressed.connect(boost::bind(&NewGamePlayerNameScreen::keyboardDELButtonPressed, this));
}

void NewGamePlayerNameScreen::initialize(const MenuScreenBaseInfo &info) {
    MenuScreenBase::initializeBase(info);
    m_textService = info.textService;
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

void NewGamePlayerNameScreen::processEvents(SDL_Event &e) {
    if (e.type == SDL_KEYUP) {
        SDL_Keycode key = e.key.keysym.sym;

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
    MenuScreenBase::updateBase();
    generateGLElements();
}

void NewGamePlayerNameScreen::render() {
    MenuScreenBase::renderBase();
    m_glFormService->drawQuad(m_namedObjects[TextureMainMenuLogo], m_texturesGL[TextureMainMenuLogo]);
    m_menuWindow.render();
    m_onScreenKeyboard.render();
    m_glFormService->drawText(m_enterNameObject);
}

void NewGamePlayerNameScreen::unloadGLMapObjects() {
}

void NewGamePlayerNameScreen::gameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    MenuScreenBase::gameWindowSizeChangedBase(size);
    m_location = {
        (m_screenSize.width() / 2.0F) ,
        (m_screenSize.height() / 2.0F) };
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
    m_enterNameObject.position = {m_location.x() + m_initialLocation.x(), -m_location.y() + m_initialLocation.y() };
    m_enterNameObject.text = fmt::format("Enter name: {0}", m_playerName);
    m_enterNameObject.color = GLColor::Gray;
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
    m_onScreenKeyboard.buttonCancelPress();
}

void NewGamePlayerNameScreen::buttonActionPressed() {
    m_onScreenKeyboard.buttonActionPress();
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
        playClickSound();
        return true;
    }
    playClickDisableSound();
    return false;
}

void NewGamePlayerNameScreen::keyboardCharButtonPressed(char c) {
    addPlayerNameChar(c);
}

void NewGamePlayerNameScreen::keyboardDELButtonPressed() {
    if (m_playerName.empty()) {
        playClickSound();
        SDL_Delay(500);
        backPressed();
    } else {
        removePlayerNameChar();
    }
}

}  // namespace thewarrior::ui::screens
