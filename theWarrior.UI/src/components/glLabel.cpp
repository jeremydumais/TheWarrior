#include <algorithm>
#include <fmt/format.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
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
m_textAlignment(textAlignment),
m_labelSize({0.0F, 0.0F}) {}

const std::string &GLLabel::getCaption() const {
    return m_glCaption.text;
}

float GLLabel::getScale() const {
    return m_glCaption.scale;
}

GLColor GLLabel::getColor() const {
    return m_glCaption.color;
}

Size<float> GLLabel::getSize() const {
    return m_labelSize;
}

void GLLabel::setCaption(const std::string &caption) {
    m_glCaption.text = caption;
    generateCaption();
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
    const float MAXWIDTH = m_screenSize.width() - 80.0F;
    // Split the message with \r
    m_glMessageLines.clear();
    std::istringstream iss(m_glCaption.text);
    std::string rawLine;
    size_t lineIndex = 0;
    float totalHeight = 0.0F;
    float longerLineWidth = 0.0F;
    while (std::getline(iss, rawLine)) {
        if (!rawLine.empty() && rawLine.back() == '\r') rawLine.pop_back();

        // Wrap this logical line into multiple visual lines
        const auto wrappedLines = wrapLineByWidth(rawLine, MAXWIDTH, m_glCaption.scale);

        for (const auto& line : wrappedLines) {
            auto lineSize = m_textService->getTextSize(line, m_glCaption.scale);

            if (lineIndex > 0) {
                totalHeight += LINESPACING;
            }
            totalHeight += lineSize.height();
            Point<float> position = {
                m_location.x() + m_initialLocation.x(),
                m_location.y() + m_initialLocation.y() + totalHeight
            };
            switch (m_textAlignment) {
                case TextAlignment::Left:
                    position.setX(position.x());
                    break;
                case TextAlignment::Center:
                    position.setX(position.x() - (lineSize.width() / 2.0F));
                    break;
                case TextAlignment::Right:
                    position.setX(position.x() - lineSize.width());
                    break;
            }
            m_glMessageLines.push_back({ std::move(line), position, m_glCaption.scale, m_glCaption.color });
            longerLineWidth = std::max(longerLineWidth, lineSize.width());
            lineIndex++;
        }
    }
    m_labelSize = { longerLineWidth, totalHeight };
    // Recenter vertically every lines
    if (m_glMessageLines.size() == 1) {
        for (auto &messageLine : m_glMessageLines) {
            messageLine.position.setY(messageLine.position.y() - (totalHeight / 2.0F));
        }
    } else {
        for (auto &messageLine : m_glMessageLines) {
            messageLine.position.setY(messageLine.position.y() + LINESPACING);
        }
    }
}

std::vector<std::string> GLLabel::wrapLineByWidth(const std::string& input,
                                                  float maxWidth,
                                                  float scale) const {
    // If it's already empty, keep a single empty line
    if (input.empty()) return {""};

    // If it already fits, keep it
    if (m_textService->getTextSize(input, scale).width() <= maxWidth) {
        return {input};
    }

    std::istringstream wordsStream(input);
    std::string word;

    std::vector<std::string> out;
    std::string current;

    while (wordsStream >> word) {
        if (current.empty()) {
            current = word;
        } else {
            std::string candidate = current + " " + word;
            if (m_textService->getTextSize(candidate, scale).width() <= maxWidth) {
                current = std::move(candidate);
            } else {
                // push current line, start a new one with the word
                out.push_back(current);
                current = word;
            }
        }
    }

    if (!current.empty()) out.push_back(current);

    // Edge case: input had only spaces (operator>> would read nothing)
    if (out.empty()) out.push_back("");

    return out;
}

}  // namespace thewarrior::ui::components
