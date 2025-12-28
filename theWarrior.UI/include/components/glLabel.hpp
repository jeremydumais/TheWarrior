#pragma once

#include <string>
#include <vector>
#include "glColor.hpp"
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glTextService.hpp"
#include "point.hpp"
#include "size.hpp"
#include <boost/signals2.hpp>

namespace thewarrior::ui::components {

enum class TextAlignment {
    Left,
    Center,
    Right
};

class GLLabel : public GLComponentBase {
 public:
    GLLabel(GLContext &glContext,
            const std::string &title,
            thewarrior::models::Point<float> location,
            const GLColor color = GLColor::White,
            const float scale = 0.6F,
            const TextAlignment textAlignment = TextAlignment::Center);
    ~GLLabel() override = default;
    const std::string &getCaption() const;
    float getScale() const;
    GLColor getColor() const;
    bool getAutoSize() const;
    void setCaption(const std::string &caption);
    void setScale(float scale);
    void setColor(GLColor color);
    void setTextAlignement(TextAlignment textAlignement);
    void setAutoSize(bool value);

 protected:
    GLTextObject m_glCaption;
    std::vector<GLTextObject> m_glMessageLines;
    TextAlignment m_textAlignment;
    bool m_autoSize;
    void onInitialize(const GLComponentBaseInfo &info) override;
    void onGenerateGLElements() override;
    void onRender() override;
    void onGameWindowSizeChanged(const thewarrior::models::Size<int> &) override;
    void generateCaption();
};

}  // namespace thewarrior::ui::components
