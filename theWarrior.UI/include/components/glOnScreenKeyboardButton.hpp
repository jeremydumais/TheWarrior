#pragma once

#include <memory>
#include <string>
#include <vector>
#include "glFormService.hpp"
#include "glObjectService.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include "point.hpp"
#include "size.hpp"
#include <boost/signals2.hpp>

namespace thewarrior::ui::components {

class GLOnScreenKeyboardButton {
 public:
    explicit GLOnScreenKeyboardButton(const std::string &caption,
                          thewarrior::models::Point<float> location,
                          thewarrior::models::Size<float> size = thewarrior::models::Size<float>(64.0F, 64.0F));
    virtual ~GLOnScreenKeyboardButton() = default;
    void initialize(const std::shared_ptr<GLTexture> texture,
                    const std::shared_ptr<GLShaderProgram> shaderProgram,
                    std::shared_ptr<GLTextService> textService);
    void setCaption(const std::string &title);
    void generateGLElements();
    void render();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
    void setTextureBeginId(int value);
    void setHasFocus(bool value);
    boost::signals2::signal<void()> onClickEvent;

 protected:
    thewarrior::models::Point<float> m_location;
    thewarrior::models::Point<float> m_initialLocation;
    thewarrior::models::Size<float> m_size;
    thewarrior::models::Point<float> m_windowCenter;
    thewarrior::models::Size<float> m_screenSize;
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
    std::shared_ptr<GLFormService> m_glFormService;
    std::shared_ptr<GLTextService> m_textService;
    GLTextureService m_textureService;
    std::shared_ptr<GLTexture> m_windowGLTexture;
    GLObject m_glwindow;
    GLTextObject m_glCaption;
    std::vector<GLObject> m_windowObjects;
    std::vector<GLObject> m_windowBackgrounds;
    int m_textureBeginId;
    bool m_hasFocus;
    void generateCaption();
};

}  // namespace thewarrior::ui::components
