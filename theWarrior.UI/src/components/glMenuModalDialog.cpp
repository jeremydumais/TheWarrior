#include <SDL2/SDL_timer.h>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "glMenuModalDialog.hpp"
#include "glColor.hpp"
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glTextService.hpp"
#include "point.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLMenuModalDialog::GLMenuModalDialog(GLContext &glContext,
                                     Point<float> location,
                                     Size<float> size)
: GLComponentBase(glContext, location, size),
m_glMessageLines(std::vector<GLTextObject>({{ "", Point<float>(0.0F, 0.0F), 0.6F, GLColor::Gray }})),
m_visible(false),
m_autoSize(true),
m_menuWindow(size),
m_menuButtonOK(glContext, "OK", Point<float>(0.0F, -135.0F), Size<float>(250.0F, 75.0F)) {
    registerComponent(&m_menuButtonOK);
}

void GLMenuModalDialog::onInitialize(const GLComponentBaseInfo &) {
    m_menuWindow.initShader(m_shaderProgram);
    m_menuWindow.initialize("", m_glTexture, m_textService);
    m_menuWindow.setTextureBeginId(29);
    m_menuWindow.setFillCenter(true);
    m_menuButtonOK.setHasFocus(true);
}

bool GLMenuModalDialog::isVisible() const {
    return m_visible;
}

bool GLMenuModalDialog::isAutoSize() const {
    return m_autoSize;
}

void GLMenuModalDialog::show() {
    m_visible = true;
}

void GLMenuModalDialog::hide() {
    m_visible = false;
}

void GLMenuModalDialog::setAutoSize(bool value) {
    m_autoSize = value;
}

void GLMenuModalDialog::setMessage(const std::string &message) {
    // Split the message with \r
    m_glMessageLines.clear();
    std::istringstream iss(message);
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        m_glMessageLines.push_back({ std::move(line), Point<float>(0.0F, 0.0F), 0.6F, GLColor::Brown });
    }
    generateGLElements();
}

void GLMenuModalDialog::onGenerateGLElements() {
    Size<float> maxSize(0.0F, 0.0F);
    const float LINESPACING = 10.0F;
    const float MARGINSPACING = 40.0F;
    // Find the largest line width and height
    for (const auto &lines : m_glMessageLines) {
        auto lineSize = m_textService->getTextSize(lines.text, 0.6F);
        if (lineSize.width() > maxSize.width()) maxSize.setWidth(lineSize.width());
        if (lineSize.height() > maxSize.height()) maxSize.setHeight(lineSize.height());
    }
    // If the text width is smaller thant the button width, use the button width
    if (maxSize.width() < m_menuButtonOK.getSize().width()) {
        maxSize.setWidth(m_menuButtonOK.getSize().width());
    }
    float contentHeight = MARGINSPACING +
        static_cast<float>(m_glMessageLines.size()) * (maxSize.height() + LINESPACING)
        + m_menuButtonOK.getSize().height()
        + MARGINSPACING;
    float linePosition = contentHeight / 2.0F - MARGINSPACING;
    for (auto &lines : m_glMessageLines) {
        auto lineSize = m_textService->getTextSize(lines.text, 0.6F);
        lines.position = {
            m_location.x() + m_initialLocation.x() + (m_screenSize.width() / 2.0F) - (lineSize.width() / 2.0F),
            m_location.y() + m_initialLocation.y() + (m_screenSize.height() / 2.0F) + (lineSize.height() / 2.0F) - linePosition
        };
        linePosition -= maxSize.height() + LINESPACING;
    }
    if (m_autoSize) {
        m_menuWindow.setWindowSize(Size<float>(maxSize.width() + (MARGINSPACING * 2.0F),
                                               contentHeight));
    }
    m_menuWindow.generateGLElements();
    m_menuButtonOK.setLocation(Point<float>(m_menuButtonOK.getLocation().x(),
                                            (contentHeight / 2.0F) - (m_menuButtonOK.getSize().height() / 2.0F) - MARGINSPACING));
}

void GLMenuModalDialog::onRender() {
    if (m_visible) {
        m_menuWindow.render();
        m_menuButtonOK.render();
        for (const auto &lines : m_glMessageLines) {
            m_glFormService->drawText(lines);
        }
    }
}

void GLMenuModalDialog::onGameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    m_menuWindow.gameWindowSizeChanged(size);
    m_glFormService->gameWindowSizeChanged(size);
}

void GLMenuModalDialog::onButtonActionPressed() {
    hide();
    playClickSound();
    SDL_Delay(200);
    onClosed();
}

void GLMenuModalDialog::onButtonCancelPressed() {
    hide();
    playBackSound();
    SDL_Delay(200);
    onClosed();
}

}  // namespace thewarrior::ui::components
