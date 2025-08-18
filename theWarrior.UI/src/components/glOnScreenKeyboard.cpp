#include <fmt/format.h>
#include <array>
#include <memory>
#include <ranges>
#include <string>
#include "glOnScreenKeyboard.hpp"
#include "glOnScreenKeyboardButton.hpp"
#include "glTexture.hpp"
#include "point.hpp"
#include <boost/algorithm/string.hpp>

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
m_enterNameObject({"Enter name:", {-200.0F, -450.0F}, 0.8F}) {
    size_t indexChar = 0;
    std::array<std::string, 43> chars = {
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
            m_buttonRows[i].push_back(std::make_shared<GLOnScreenKeyboardButton>(chars.at(indexChar), Point<float>(buttonLeft, buttonTop)));
            buttonLeft += 77.0F;
            indexChar++;
        }
        buttonTop += 77.0F;
    }
    m_buttonRows[4].push_back(std::make_shared<GLOnScreenKeyboardButton>(chars.at(40),
                              Point<float>(-318.0F, 158.0F),
                              Size<float>(128.0F, 61.5F)));
    m_buttonRows[4].push_back(std::make_shared<GLOnScreenKeyboardButton>(chars.at(41),
                              Point<float>(-5.0F, 158.0F),
                              Size<float>(470.0F, 61.5F)));
    m_buttonRows[4].push_back(std::make_shared<GLOnScreenKeyboardButton>(chars.at(42),
                              Point<float>(311.0F, 158.0F),
                              Size<float>(128.0F, 61.5F)));
}

void GLOnScreenKeyboard::initialize(const std::shared_ptr<GLTexture> glTexture,
                              const std::shared_ptr<GLShaderProgram> shaderProgram,
                              std::shared_ptr<GLTextService> textService) {
    m_windowGLTexture = glTexture;
    m_shaderProgram = shaderProgram;
    m_textService = textService;
    m_glFormService->initialize(m_shaderProgram, textService);
    for (auto& row : m_buttonRows | std::views::all) {
        for (auto& button : row | std::views::all) {
            button->initialize(glTexture, shaderProgram, textService);
        }
    }
}

void GLOnScreenKeyboard::generateGLElements() {
    for (auto& row : m_buttonRows | std::views::all) {
        for (auto& button : row | std::views::all) {
            button->generateGLElements();
        }
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
    for (auto& row : m_buttonRows | std::views::all) {
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
    for (auto& row : m_buttonRows | std::views::all) {
        for (auto& button : row | std::views::all) {
            button->gameWindowSizeChanged(size);
        }
    }
}

}  // namespace thewarrior::ui::components
