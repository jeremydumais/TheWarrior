#pragma once

#include <string>
#include "glColor.hpp"
#include "glComponentBase.hpp"
#include "glTextService.hpp"
#include "point.hpp"
#include "size.hpp"
#include <boost/signals2.hpp>

namespace thewarrior::ui::components {

class GLLabel : public GLComponentBase {
 public:
    GLLabel(const std::string &title,
            thewarrior::models::Point<float> location,
            const GLColor color = GLColor::White,
            const float scale = 0.6F);
    ~GLLabel() override;
    void initialize(const GLComponentBaseInfo &info);
    void setCaption(const std::string &title);
    void onGenerateGLElements() override;
    void onRender() override;
    void onGameWindowSizeChanged(const thewarrior::models::Size<int> &) override;

 protected:
    GLTextObject m_glCaption;
    void generateCaption();
};

}  // namespace thewarrior::ui::components
