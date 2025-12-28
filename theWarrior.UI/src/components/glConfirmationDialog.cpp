#include <SDL2/SDL_timer.h>
#include <string>
#include <vector>
#include "glConfirmationDialog.hpp"
#include "glColor.hpp"
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glTextService.hpp"
#include "point.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLConfirmationDialog::GLConfirmationDialog(GLContext &glContext,
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
m_menuButtonOK(glContext, "OK", Point<float>(0.0F, 0.0F), Size<float>(250.0F, 75.0F)),
m_menuButtonCancel(glContext, "Cancel", Point<float>(0.0F, 0.0F), Size<float>(250.0F, 75.0F)) {
    registerComponent(&m_menuPanel);
    registerComponent(&m_messageLabel);
    registerComponent(&m_menuButtonOK);
    registerComponent(&m_menuButtonCancel);
}

void GLConfirmationDialog::onInitialize(const GLComponentBaseInfo &) {
    m_menuButtonOK.setHasFocus(true);
}

bool GLConfirmationDialog::isVisible() const {
    return m_visible;
}

void GLConfirmationDialog::show() {
    m_visible = true;
}

void GLConfirmationDialog::hide() {
    m_visible = false;
}

void GLConfirmationDialog::setMessage(const std::string &message) {
    m_messageLabel.setCaption(message);
    generateGLElements();
}

void GLConfirmationDialog::onGenerateGLElements() {
    const float MARGINSPACING = 40.0F;
    const auto LABELSIZE = m_messageLabel.getSize();
    const auto MINIMUMPANELWIDTH = m_menuButtonOK.getSize().width() + m_menuButtonCancel.getSize().width() + (MARGINSPACING * 3.0F);
    Size<float> newPanelSize = {
        LABELSIZE.width() + (MARGINSPACING * 2.0F),
        LABELSIZE.height() + MARGINSPACING + m_menuButtonOK.getSize().height() + 60.0F
    };

    if (newPanelSize.width() < MINIMUMPANELWIDTH) {
        newPanelSize.setWidth(MINIMUMPANELWIDTH);
    }

    m_menuPanel.setSize(newPanelSize);
    m_messageLabel.setLocation({ m_messageLabel.getLocation().x(),
                                 -(LABELSIZE.height()) - (MARGINSPACING / 2.0F) });
    m_menuButtonOK.setLocation({ (m_menuButtonOK.getSize().width() / 2.0F) + (MARGINSPACING / 2.0F),
                                 (LABELSIZE.height() / 2.0F) + (MARGINSPACING / 2.0F) });
    m_menuButtonCancel.setLocation({ -(m_menuButtonCancel.getSize().width() / 2.0F) - (MARGINSPACING / 2.0F),
                                 (LABELSIZE.height() / 2.0F) + (MARGINSPACING / 2.0F) });
}

void GLConfirmationDialog::onRender() {
    if (m_visible) {
        m_menuPanel.render();
        m_messageLabel.render();
        m_menuButtonOK.render();
        m_menuButtonCancel.render();
    }
}

void GLConfirmationDialog::onGameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    m_glFormService->gameWindowSizeChanged(size);
}

void GLConfirmationDialog::onButtonLeftPressed() {
    if (m_menuButtonOK.hasFocus()) {
        playMoveSound();
        m_menuButtonOK.setHasFocus(false);
        m_menuButtonCancel.setHasFocus(true);
    }
}

void GLConfirmationDialog::onButtonRightPressed() {
    if (m_menuButtonCancel.hasFocus()) {
        playMoveSound();
        m_menuButtonCancel.setHasFocus(false);
        m_menuButtonOK.setHasFocus(true);
    }
}

void GLConfirmationDialog::onButtonActionPressed() {
    if (m_menuButtonOK.hasFocus()) {
        hide();
        playClickSound();
        SDL_Delay(200);
        onClosed(ConfirmationDialogResult::OK);
    } else {
        cancelAction();
    }
}

void GLConfirmationDialog::onButtonCancelPressed() {
    cancelAction();
}

void GLConfirmationDialog::cancelAction() {
    hide();
    playBackSound();
    SDL_Delay(200);
}

}  // namespace thewarrior::ui::components
