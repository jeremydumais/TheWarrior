#include <fmt/format.h>
#include <cctype>
#include <array>
#include <cstddef>
#include <memory>
#include <ranges>
#include <string>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string.hpp>
#include "glOnScreenKeyboard.hpp"
#include "glColor.hpp"
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glLabel.hpp"
#include "mainMenuCommons.hpp"
#include "point.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::components {

namespace {
constexpr float KeyboardSourceWidth = 1290.0F;
constexpr float KeyboardSourceHeight = 451.0F;
constexpr int KeyboardDisplayWidth = 840;
constexpr int KeyboardDisplayHeight = 353;

// Centers of the key faces in mainmenu_keyboard.png, including its padding.
constexpr std::array<float, 10> KeyColumnCenters = {
    101.0F, 224.0F, 345.0F, 466.0F, 589.0F,
    710.0F, 833.0F, 955.0F, 1072.0F, 1190.5F
};
constexpr std::array<float, 4> KeyRowCenters = {59.0F, 135.0F, 211.0F, 286.0F};
constexpr std::array<float, 3> ActionKeyCenters = {272.0F, 647.0F, 1027.0F};
constexpr float ActionKeyY = 375.0F;
}  // namespace


GLOnScreenKeyboard::GLOnScreenKeyboard(GLContext &glContext, Point<float> location)
: GLComponentBase(glContext, location, Size<float>(700.0F, 700.0F)),
m_focusPosition(0, 0),
m_fourthRowLastXPosition(0),
m_isInCapsMode(false) {
}

GLOnScreenKeyboard::~GLOnScreenKeyboard() {
    unloadTexture(TextureMainMenuKeyboard);
}

void GLOnScreenKeyboard::onInitialize(const GLComponentBaseInfo &) {
    if (!loadTextures()) {
        throw std::runtime_error(getLastError());
    }
}

bool GLOnScreenKeyboard::loadTextures() {
    return loadTexture(TextureMainMenuKeyboard, "mainmenu_keyboard.png", 1290, 451);
}

void GLOnScreenKeyboard::onGenerateGLElements() {
    generateGLObject(TextureMainMenuKeyboard,
                     Size<>(KeyboardDisplayWidth, KeyboardDisplayHeight),
                     components::HorizontalAlignment::Center,
                     components::VerticalAlignment::Center,
                     Point<>(static_cast<int>(m_initialLocation.x()),
                             static_cast<int>(m_initialLocation.y())));
    generateKeyboardItems();
    auto buttonPosition = Point<size_t>(0, 0);
    for (const auto& row : m_buttonRows | std::views::all) {
        for (auto& button : row | std::views::all) {
            if (buttonPosition == m_focusPosition) {
                button->setColor(GLColor::Green);
            } else {
                button->setColor(GLColor::Brown);
            }
            button->generateGLElements();
            buttonPosition.setX(buttonPosition.x() + 1);
        }
        buttonPosition.setX(0);
        buttonPosition.setY(buttonPosition.y() + 1);
    }
}

void GLOnScreenKeyboard::onRender() {
    drawGLObject(TextureMainMenuKeyboard);
    for (const auto& row : m_buttonRows | std::views::all) {
        for (auto& button : row | std::views::all) {
            button->render();
        }
    }
}

void GLOnScreenKeyboard::onGameWindowSizeChanged(const Size<> &size) {
    m_location = {
        (m_screenSize.width() / 2.0F) - (m_size.width() / 2.0F) + m_initialLocation.x(),
        (m_screenSize.height() / 2.0F) - (m_size.height() / 2.0F) + m_initialLocation.y() };
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
            if (m_focusPosition.x() <= 2) {
                m_focusPosition.setX(0);
            } else if (m_focusPosition.x() >= 7) {
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

void GLOnScreenKeyboard::buttonCancelPress() {
    onDELButtonPressed();
}

void GLOnScreenKeyboard::buttonActionPress() {
    // Shift button
    if (m_focusPosition == Point<size_t>(0, 4)) {
        m_isInCapsMode = !m_isInCapsMode;
        generateKeyboardItems();
        generateGLElements();
    } else if (m_focusPosition == Point<size_t>(1, 4)) {
        onCharButtonPressed(' ');
    } else if (m_focusPosition == Point<size_t>(2, 4)) {
        onOKButtonPressed();
    } else if (m_focusPosition == Point<size_t>(9, 3)) {
        onDELButtonPressed();
    } else {
        char c = ONSCREENKEYBOARD_BUTTONSTEXT.at(m_focusPosition.y() * 10 + m_focusPosition.x()).at(0);
        if (m_isInCapsMode) {
            c = static_cast<char>(std::toupper(c));
        }
        onCharButtonPressed(c);
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
    const auto keyPosition = [this](float sourceX, float sourceY) -> Point<float> {
        return {
            (sourceX / KeyboardSourceWidth - 0.5F) * KeyboardDisplayWidth + m_initialLocation.x(),
            (sourceY / KeyboardSourceHeight - 0.5F) * KeyboardDisplayHeight + m_initialLocation.y()
        };
    };
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 10; j++) {
            std::string buttonText = std::string(ONSCREENKEYBOARD_BUTTONSTEXT.at(indexChar));
            if (m_isInCapsMode) {
                buttonText = boost::to_upper_copy(buttonText);
            }
            auto button = std::make_shared<GLLabel>(m_glContext,
                                                    buttonText,
                                                    keyPosition(KeyColumnCenters[j], KeyRowCenters[i]),
                                                    GLColor::Brown);
            m_buttonRows[i].push_back(button);
            indexChar++;
        }
    }
    for (size_t i = 0; i < ActionKeyCenters.size(); ++i) {
        m_buttonRows[4].push_back(std::make_shared<GLLabel>(
            m_glContext, std::string(ONSCREENKEYBOARD_BUTTONSTEXT.at(40 + i)),
            keyPosition(ActionKeyCenters[i], ActionKeyY), GLColor::Brown));
    }
    for (const auto& row : m_buttonRows | std::views::all) {
        for (auto& button : row | std::views::all) {
            button->initialize(getComponentBaseInfo());
            button->gameWindowSizeChanged(Size<>(static_cast<int>(m_screenSize.width()),
                                                 static_cast<int>(m_screenSize.height())));
        }
    }
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
