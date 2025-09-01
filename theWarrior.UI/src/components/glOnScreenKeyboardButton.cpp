#include <fmt/format.h>
#include <memory>
#include <string>
#include <vector>
#include "glOnScreenKeyboardButton.hpp"
#include "floatUtils.hpp"
#include "glColor.hpp"
#include "glComponentBase.hpp"
#include "glTexture.hpp"
#include "point.hpp"
#include <boost/algorithm/string.hpp>

using namespace thewarrior::models;
using thewarrior::utils::FloatUtils;

namespace thewarrior::ui::components {

GLOnScreenKeyboardButton::GLOnScreenKeyboardButton(const std::string &caption, Point<float> location, Size<float> size)
: GLComponentBase(location, size),
m_size(size),
m_glCaption({caption, {1.0F, 1.0F}, 0.6F}),
m_windowObjects(std::vector<GLObject>()),
m_windowBackgrounds(std::vector<GLObject>()),
m_textureBeginId(48),
m_hasFocus(false) {}

GLOnScreenKeyboardButton::~GLOnScreenKeyboardButton() {
    GLComponentBase::freeGLObjects(m_windowObjects);
    GLComponentBase::freeGLObjects(m_windowBackgrounds);
}

void GLOnScreenKeyboardButton::initialize(const GLComponentBaseInfo &info) {
    GLComponentBase::initialize(info);
}

void GLOnScreenKeyboardButton::setCaption(const std::string &title) {
    m_glCaption.text = title;
    generateCaption();
}

void GLOnScreenKeyboardButton::onGenerateGLElements() {
    GLComponentBase::freeGLObjects(m_windowObjects);
    GLComponentBase::freeGLObjects(m_windowBackgrounds);
    if (FloatUtils::areEqual(m_size.width(), m_size.height())) {
        m_glFormService->generateQuad(m_windowBackgrounds,
                {m_initialLocation.x() + m_location.x(), m_initialLocation.y() + m_location.y()},
                m_size, &m_glTexture->texture, 47);
    } else {
        m_glFormService->generateQuad(m_windowBackgrounds,
                {m_initialLocation.x() + m_location.x(), m_initialLocation.y() + m_location.y()},
                m_size, &m_glTexture->texture, m_textureBeginId + 8);
        m_glFormService->generateBoxQuad(m_windowObjects,
                {m_initialLocation.x() + m_location.x(), m_initialLocation.y() + m_location.y()},
                m_size, &m_glTexture->texture, m_textureBeginId);
    }
    generateCaption();
}

void GLOnScreenKeyboardButton::onRender() {
    for (const auto &obj : m_windowBackgrounds) {
        m_glFormService->drawQuad(obj, m_glTexture->glTextureId, 1.0F);
    }
    for (const auto &obj : m_windowObjects) {
        m_glFormService->drawQuad(obj, m_glTexture->glTextureId);
    }
    m_glFormService->drawText(m_glCaption);
}

void GLOnScreenKeyboardButton::onGameWindowSizeChanged(const Size<> &) {
    m_location = {
        (m_screenSize.width() / 2.0F) - (m_size.width() / 2.0F),
        (m_screenSize.height() / 2.0F) - (m_size.height() / 2.0F) };
}

void GLOnScreenKeyboardButton::setTextureBeginId(int value) {
    m_textureBeginId = value;
}

void GLOnScreenKeyboardButton::setHasFocus(bool value) {
    m_hasFocus = value;
}

void GLOnScreenKeyboardButton::generateCaption() {
    auto titleSize = m_textService->getTextSize(m_glCaption.text, 0.6F);
    m_glCaption.position = {m_location.x() + m_initialLocation.x() +
        (m_size.width() / 2.0F) -
            (titleSize.width() / 2.0F),
            m_location.y() + m_initialLocation.y() +
                (m_size.height() / 2.0F) +
                (titleSize.height() / 2.0F)};
    if (m_hasFocus) {
        m_glCaption.color = GLColor::Green;
    } else {
        m_glCaption.color = GLColor::White;
    }
}

}  // namespace thewarrior::ui::components
