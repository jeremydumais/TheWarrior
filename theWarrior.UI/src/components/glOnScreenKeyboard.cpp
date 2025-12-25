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
#include "textureInfo.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLOnScreenKeyboard::GLOnScreenKeyboard(GLContext &glContext, Point<float> location)
: GLComponentBase(glContext, location, Size<float>(700.0F, 700.0F)),
m_focusPosition(0, 0),
m_fourthRowLastXPosition(0),
m_isInCapsMode(false) {
}

GLOnScreenKeyboard::~GLOnScreenKeyboard() {
    unloadTexture(TextureMainMenuKeyboard);
}

void GLOnScreenKeyboard::initialize(const GLComponentBaseInfo &info) {
    GLComponentBase::initialize(info);
    if (!loadTextures()) {
        throw std::runtime_error(getLastError());
    }
    generateGLElements();
}

bool GLOnScreenKeyboard::loadTextures() {
    return loadTexture(TextureMainMenuKeyboard, "mainmenu_keyboard.png", 1290, 451);
}

void GLOnScreenKeyboard::onGenerateGLElements() {
    generateGLObject(TextureMainMenuKeyboard,
                     Size<>(840, 353),
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
    float buttonTop = -136.0F + m_initialLocation.y();
    for (size_t i = 0; i < 4; i++) {
        float buttonLeft = -374.0F + m_initialLocation.x();
        for (size_t j = 0; j < 10; j++) {
            std::string buttonText = std::string(ONSCREENKEYBOARD_BUTTONSTEXT.at(indexChar));
            if (m_isInCapsMode) {
                buttonText = boost::to_upper_copy(buttonText);
            }
            auto button = std::make_shared<GLLabel>(m_glContext,
                                                    buttonText,
                                                    Point<float>(buttonLeft, buttonTop),
                                                    GLColor::Brown);
            m_buttonRows[i].push_back(button);
            buttonLeft += 83.0F;
            indexChar++;
        }
        buttonTop += 62.0F;
    }
    m_buttonRows[4].push_back(std::make_shared<GLLabel>(m_glContext, std::string(ONSCREENKEYBOARD_BUTTONSTEXT.at(40)),
                              Point<float>(-258.0F + m_initialLocation.x(), 124.0F + m_initialLocation.y()),
                              GLColor::Brown));
    m_buttonRows[4].push_back(std::make_shared<GLLabel>(m_glContext, std::string(ONSCREENKEYBOARD_BUTTONSTEXT.at(41)),
                              Point<float>(0.0F + m_initialLocation.x(), 124.0F + m_initialLocation.y()),
                              GLColor::Brown));
    m_buttonRows[4].push_back(std::make_shared<GLLabel>(m_glContext, std::string(ONSCREENKEYBOARD_BUTTONSTEXT.at(42)),
                              Point<float>(260.0F + m_initialLocation.x(), 124.0F + m_initialLocation.y()),
                              GLColor::Brown));
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
