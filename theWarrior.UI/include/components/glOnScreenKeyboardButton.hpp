#pragma once

#include <string>
#include <vector>
#include "glComponentBase.hpp"
#include "glObjectService.hpp"
#include "glTextService.hpp"
#include "point.hpp"
#include "size.hpp"
#include <boost/signals2.hpp>

namespace thewarrior::ui::components {

class GLOnScreenKeyboardButton : public GLComponentBase {
 public:
    GLOnScreenKeyboardButton(const std::string &caption,
                             thewarrior::models::Point<float> location,
                             thewarrior::models::Size<float> size = thewarrior::models::Size<float>(64.0F, 64.0F));
    ~GLOnScreenKeyboardButton() override;
    void initialize(const GLComponentBaseInfo &info);
    void setCaption(const std::string &title);
    void onGenerateGLElements() override;
    void onRender() override;
    void onGameWindowSizeChanged(const thewarrior::models::Size<> &) override;
    void setTextureBeginId(int value);
    void setHasFocus(bool value);
    boost::signals2::signal<void()> onClickEvent;

 protected:
    thewarrior::models::Size<float> m_size;
    GLTextObject m_glCaption;
    std::vector<GLObject> m_windowObjects;
    std::vector<GLObject> m_windowBackgrounds;
    int m_textureBeginId;
    bool m_hasFocus;
    void generateCaption();
};

}  // namespace thewarrior::ui::components
