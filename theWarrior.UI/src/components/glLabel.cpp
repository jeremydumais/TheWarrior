#include <fmt/format.h>
#include <string>
#include "glLabel.hpp"
#include "glColor.hpp"
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "point.hpp"
#include <boost/algorithm/string.hpp>

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLLabel::GLLabel(GLContext &glContext,
                 const std::string &caption,
                 Point<float> location,
                 const GLColor color,
                 const float scale)
: GLComponentBase(glContext, location),
m_glCaption({caption, {1.0F, 1.0F}, scale, color}) {}

GLLabel::~GLLabel() {
}

void GLLabel::initialize(const GLComponentBaseInfo &info) {
    GLComponentBase::initialize(info);
}

void GLLabel::setCaption(const std::string &title) {
    m_glCaption.text = title;
    generateCaption();
}

void GLLabel::setColor(GLColor color) {
    m_glCaption.color = color;
}

void GLLabel::onGenerateGLElements() {
    generateCaption();
}

void GLLabel::onRender() {
    m_glFormService->drawText(m_glCaption);
}

void GLLabel::onGameWindowSizeChanged(const Size<int> &size) {
    m_location = {
        (m_screenSize.width() / 2.0F) - (m_size.width() / 2.0F),
        (m_screenSize.height() / 2.0F) - (m_size.height() / 2.0F) };
    m_glFormService->gameWindowSizeChanged(size);
}

void GLLabel::generateCaption() {
    auto titleSize = m_textService->getTextSize(m_glCaption.text, m_glCaption.scale);
    m_glCaption.position = {m_location.x() + m_initialLocation.x() +
        (m_size.width() / 2.0F) -
            (titleSize.width() / 2.0F),
            m_location.y() + m_initialLocation.y() +
                (m_size.height() / 2.0F) +
                (titleSize.height() / 2.0F)};
}

}  // namespace thewarrior::ui::components
