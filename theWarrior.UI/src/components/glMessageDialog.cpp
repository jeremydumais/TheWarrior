#include <SDL2/SDL_timer.h>
#include <string>
#include <vector>
#include "glMessageDialog.hpp"
#include "glColor.hpp"
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glTextService.hpp"
#include "point.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLMessageDialog::GLMessageDialog(GLContext &glContext,
                                     Point<float> location,
                                     Size<float> size)
: GLComponentBase(glContext, location, size),
m_glMessageLines(std::vector<GLTextObject>({{ "", Point<float>(0.0F, 0.0F), 0.6F, GLColor::Gray }})),
m_visible(false),
m_menuPanel(glContext, size, {
    .location = location,
    .skin = NineSliceSkin {}
}),
m_messageLabel(glContext, "", Point<float>(0.0F, 0.0F), GLColor::Brown),
m_menuButtonOK(glContext, "OK", Point<float>(0.0F, 0.0F), Size<float>(250.0F, 75.0F)) {
    registerComponent(&m_menuPanel);
    registerComponent(&m_messageLabel);
    registerComponent(&m_menuButtonOK);
}

void GLMessageDialog::onInitialize(const GLComponentBaseInfo &) {
    m_messageLabel.setAutoSize(true);
    m_menuButtonOK.setHasFocus(true);
}

bool GLMessageDialog::isVisible() const {
    return m_visible;
}

void GLMessageDialog::show() {
    m_visible = true;
}

void GLMessageDialog::hide() {
    m_visible = false;
}

void GLMessageDialog::setMessage(const std::string &message) {
    m_messageLabel.setCaption(message);
}

void GLMessageDialog::onGenerateGLElements() {
    const float MARGINSPACING = 40.0F;
    const auto labelSize = m_messageLabel.getSize();

    m_menuPanel.setSize({
            labelSize.width() + (MARGINSPACING * 2.0F),
            labelSize.height() + MARGINSPACING + m_menuButtonOK.getSize().height() + 60.0F
            });
    m_messageLabel.setLocation({ (m_menuPanel.getSize().width() / 2.0F) - MARGINSPACING,
                                 -labelSize.height() / 2.0F });
    m_menuButtonOK.setLocation({ m_menuButtonOK.getLocation().x(),
                                 (labelSize.height() / 2.0F) + (MARGINSPACING / 2.0F) });
}

void GLMessageDialog::onRender() {
    if (m_visible) {
        m_menuPanel.render();
        m_messageLabel.render();
        m_menuButtonOK.render();
    }
}

void GLMessageDialog::onGameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    m_glFormService->gameWindowSizeChanged(size);
}

void GLMessageDialog::onButtonActionPressed() {
    hide();
    playClickSound();
    SDL_Delay(200);
    onClosed();
}

void GLMessageDialog::onButtonCancelPressed() {
    hide();
    playBackSound();
    SDL_Delay(200);
    onClosed();
}

}  // namespace thewarrior::ui::components
