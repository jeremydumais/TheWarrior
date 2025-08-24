#include <fmt/format.h>
#include <array>
#include <cstddef>
#include <memory>
#include <ranges>
#include <string>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string.hpp>
#include "glOnScreenKeyboard.hpp"
#include "glOnScreenKeyboardButton.hpp"
#include "glTexture.hpp"
#include "point.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLOnScreenKeyboard::GLOnScreenKeyboard(Point<float> location)
: m_location(location),
m_initialLocation(location),
m_size(Size<float>(700.0F, 700.0F)),
m_windowCenter({1.0F, 1.0F}),
m_screenSize({1.0F, 1.0F}),
m_shaderProgram(nullptr),
m_glFormService(std::make_shared<GLFormService>()),
m_textService(nullptr),
m_windowGLTexture(nullptr),
m_enterNameObject({"Enter name:", {-200.0F, -450.0F}, 0.8F}),
m_focusPosition(0, 0),
m_fourthRowLastXPosition(0),
m_isInCapsMode(false) {
}

void GLOnScreenKeyboard::initialize(const std::shared_ptr<GLTexture> glTexture,
                              const std::shared_ptr<GLShaderProgram> shaderProgram,
                              std::shared_ptr<GLTextService> textService,
                              std::shared_ptr<Mix_Chunk> menuMoveSound,
                              std::shared_ptr<Mix_Chunk> menuClickSound) {
    m_windowGLTexture = glTexture;
    m_shaderProgram = shaderProgram;
    m_textService = textService;
    m_glFormService->initialize(m_shaderProgram, textService);
    generateKeyboardItems();
    m_menuMoveSound = menuMoveSound;
    m_menuClickSound = menuClickSound;
}

void GLOnScreenKeyboard::generateGLElements() {
    auto buttonPosition = Point<size_t>(0, 0);
    for (const auto& row : m_buttonRows | std::views::all) {
        for (auto& button : row | std::views::all) {
            if (buttonPosition == m_focusPosition) {
                button->setHasFocus(true);
            } else {
                button->setHasFocus(false);
            }
            button->generateGLElements();
            buttonPosition.setX(buttonPosition.x() + 1);
        }
        buttonPosition.setX(0);
        buttonPosition.setY(buttonPosition.y() + 1);
    }
    auto enterNameSize = m_textService->getTextSize(m_enterNameObject.text, 0.8F);
    m_enterNameObject.position = {m_location.x() + m_initialLocation.x() +
        (m_size.width() / 2.0F) -
            (enterNameSize.width() / 2.0F) - 200.0F,
            m_location.y() + m_initialLocation.y() +
                (m_size.height() / 2.0F) +
                (enterNameSize.height() / 2.0F) - 250.0F};
    m_enterNameObject.color = GLColor::Gray;
}

void GLOnScreenKeyboard::render() {
    for (const auto& row : m_buttonRows | std::views::all) {
        for (auto& button : row | std::views::all) {
            button->render();
        }
    }
    m_glFormService->drawText(m_enterNameObject);
}

void GLOnScreenKeyboard::gameWindowSizeChanged(const Size<> &size) {
    m_screenSize = Size<float>(static_cast<float>(size.width()),
                               static_cast<float>(size.height()));
    m_location = {
        (m_screenSize.width() / 2.0F) - (m_size.width() / 2.0F),
        (m_screenSize.height() / 2.0F) - (m_size.height() / 2.0F) };
    m_windowCenter = {m_location.x() + (m_size.width() / 2.0F),
                      m_location.y() + (m_size.height() / 2.0F)};
    m_glFormService->gameWindowSizeChanged(size);
    for (const auto& row : m_buttonRows | std::views::all) {
        for (auto& button : row | std::views::all) {
            button->gameWindowSizeChanged(size);
        }
    }
}

void GLOnScreenKeyboard::buttonUpPress() {
    if (m_focusPosition.y() > 0) {
        if (m_focusPosition.y() == 4) {  // Last Row
            m_focusPosition.setX(m_fourthRowLastXPosition);
        }
        m_focusPosition.setY(m_focusPosition.y() - 1);
        generateGLElements();
        playMoveSound();
    }
}

void GLOnScreenKeyboard::buttonDownPress() {
    if (m_focusPosition.y() < m_buttonRows.size() -1) {
        if (m_focusPosition.y() == 3) {  // Fourth row
            m_fourthRowLastXPosition = m_focusPosition.x();
            // Move to the right fifth row button depending on where the focus
            // was on the fourth row
            if (m_focusPosition.x() <= 1) {
                m_focusPosition.setX(0);
            } else if (m_focusPosition.x() >= 8) {
                m_focusPosition.setX(2);
            } else {
                m_focusPosition.setX(1);
            }
        }
        m_focusPosition.setY(m_focusPosition.y() + 1);
        generateGLElements();
        playMoveSound();
    }
}

void GLOnScreenKeyboard::buttonLeftPress() {
    if (m_focusPosition.x() > 0) {
        m_focusPosition.setX(m_focusPosition.x() - 1);
        if (m_focusPosition.y() == 4) {  // Fifth row
            updateFourthRowLastXPosition();
        }
        generateGLElements();
        playMoveSound();
    }
}

void GLOnScreenKeyboard::buttonRightPress() {
    if (m_focusPosition.x() < m_buttonRows.at(m_focusPosition.y()).size() - 1) {
        m_focusPosition.setX(m_focusPosition.x() + 1);
        if (m_focusPosition.y() == 4) {  // Fifth row
            updateFourthRowLastXPosition();
        }
        generateGLElements();
        playMoveSound();
    }
}

void GLOnScreenKeyboard::buttonActionPress() {
    if (m_focusPosition == Point<size_t>(0, 4)) {
        m_isInCapsMode = !m_isInCapsMode;
        generateKeyboardItems();
        generateGLElements();
    }
    playClickSound();
}

void GLOnScreenKeyboard::generateKeyboardItems() {
    m_buttonRows.at(0).clear();
    m_buttonRows.at(1).clear();
    m_buttonRows.at(2).clear();
    m_buttonRows.at(3).clear();
    m_buttonRows.at(4).clear();
    size_t indexChar = 0;
    const std::array<std::string, 43> BUTTONSTEXT = {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
        "q", "w", "e", "r", "t", "y", "u", "i", "o", "p",
        "a", "s", "d", "f", "g", "h", "j", "k", "l", "-",
        "z", "x", "c", "v", "b", "n", "m", "\'", ".", "DEL",
        "SHIFT", "SPACE", "OK"
    };
    float buttonTop = -150.0F;
    for (size_t i = 0; i < 4; i++) {
        float buttonLeft = -350.0F;
        for (size_t j = 0; j < 10; j++) {
            std::string buttonText = BUTTONSTEXT.at(indexChar);
            if (m_isInCapsMode) {
                buttonText = boost::to_upper_copy(buttonText);
            }
            auto button = std::make_shared<GLOnScreenKeyboardButton>(buttonText,
                                                                     Point<float>(buttonLeft, buttonTop));
            m_buttonRows[i].push_back(button);
            buttonLeft += 77.0F;
            indexChar++;
        }
        buttonTop += 77.0F;
    }
    m_buttonRows[4].push_back(std::make_shared<GLOnScreenKeyboardButton>(BUTTONSTEXT.at(40),
                              Point<float>(-318.0F, 158.0F),
                              Size<float>(128.0F, 61.5F)));
    m_buttonRows[4].push_back(std::make_shared<GLOnScreenKeyboardButton>(BUTTONSTEXT.at(41),
                              Point<float>(-5.0F, 158.0F),
                              Size<float>(470.0F, 61.5F)));
    m_buttonRows[4].push_back(std::make_shared<GLOnScreenKeyboardButton>(BUTTONSTEXT.at(42),
                              Point<float>(311.0F, 158.0F),
                              Size<float>(128.0F, 61.5F)));
    for (const auto& row : m_buttonRows | std::views::all) {
        for (auto& button : row | std::views::all) {
            button->initialize(m_windowGLTexture, m_shaderProgram, m_textService);
            button->gameWindowSizeChanged(Size<>(static_cast<int>(m_screenSize.width()),
                                                 static_cast<int>(m_screenSize.height())));
        }
    }
}

void GLOnScreenKeyboard::playMoveSound() {
    Mix_PlayChannel(-1, m_menuMoveSound.get(), 0);
}

void GLOnScreenKeyboard::playClickSound() {
    Mix_PlayChannel(-1, m_menuClickSound.get(), 0);
}

// The fifth row having less buttons, we need to reposition the last focus
// position of the fourth row if we are going left/right on the fifth row.
void GLOnScreenKeyboard::updateFourthRowLastXPosition() {
    if (m_focusPosition.x() == 0) {
        m_fourthRowLastXPosition = 1;
    } else if (m_focusPosition.x() == 1) {
        m_fourthRowLastXPosition = 4;
    } else {
        m_fourthRowLastXPosition = 8;
    }
}


}  // namespace thewarrior::ui::components
