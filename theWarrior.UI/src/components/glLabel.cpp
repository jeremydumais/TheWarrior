#include <fmt/format.h>
#include <sstream>
#include <string>
#include <utility>
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
                 const float scale,
                 const TextAlignment textAlignment)
: GLComponentBase(glContext, location),
m_glCaption({caption, location, scale, color}),
m_glMessageLines({}),
m_textAlignment(textAlignment) {}

GLLabel::~GLLabel() {
}

const std::string &GLLabel::getCaption() const {
    return m_glCaption.text;
}

const Point<float> &GLLabel::getPosition() const {
    return m_glCaption.position;
}

float GLLabel::getScale() const {
    return m_glCaption.scale;
}

GLColor GLLabel::getColor() const {
    return m_glCaption.color;
}

void GLLabel::setCaption(const std::string &caption) {
    m_glCaption.text = caption;
    generateCaption();
}

void GLLabel::setPosition(const Point<float> &position) {
    m_glCaption.position = position;
}

void GLLabel::setScale(float scale) {
    m_glCaption.scale = scale;
}

void GLLabel::setColor(GLColor color) {
    m_glCaption.color = color;
}

void GLLabel::setTextAlignement(TextAlignment textAlignement) {
    m_textAlignment = textAlignement;
}

void GLLabel::onInitialize(const GLComponentBaseInfo &) {
    setCaption(m_glCaption.text);
}

void GLLabel::onGenerateGLElements() {
    generateCaption();
}

void GLLabel::onRender() {
    for (const auto &lines : m_glMessageLines) {
        m_glFormService->drawText(lines);
    }
}

void GLLabel::onGameWindowSizeChanged(const Size<int> &size) {
    m_location = {
        (m_screenSize.width() / 2.0F) - (m_size.width() / 2.0F),
        (m_screenSize.height() / 2.0F) - (m_size.height() / 2.0F) };
    m_glFormService->gameWindowSizeChanged(size);
}

void GLLabel::generateCaption() {
    const float LINESPACING = 10.0F;
    // Split the message with \r
    m_glMessageLines.clear();
    std::istringstream iss(m_glCaption.text);
    std::string line;
    size_t lineIndex = 0;
    float totalHeight = 0.0F;
    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        auto titleSize = m_textService->getTextSize(line, m_glCaption.scale);

        if (lineIndex > 0) {
            totalHeight += LINESPACING;
        }
        Point<float> position = {
            m_location.x() + m_initialLocation.x(),
            m_location.y() + m_initialLocation.y() + totalHeight + (titleSize.height())
        };
        switch (m_textAlignment) {
            case TextAlignment::Left:
                position.setX(position.x());
                break;
            case TextAlignment::Center:
                position.setX(position.x() - (titleSize.width() / 2.0F));
                break;
            case TextAlignment::Right:
                position.setX(position.x() - titleSize.width());
                break;
        }
        m_glMessageLines.push_back({ std::move(line), position, m_glCaption.scale, m_glCaption.color });
        totalHeight += titleSize.height();
        lineIndex++;
    }
    // Recenter vertically every lines
    for (auto &messageLine : m_glMessageLines) {
        messageLine.position.setY(messageLine.position.y() - (totalHeight / 2.0F));
    }
}

}  // namespace thewarrior::ui::components
