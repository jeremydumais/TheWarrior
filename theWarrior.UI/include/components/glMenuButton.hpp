#pragma once

#include <string>
#include <vector>
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glObjectService.hpp"
#include "glTextService.hpp"
#include "point.hpp"
#include "size.hpp"
#include "texture.hpp"
#include <boost/signals2.hpp>

namespace thewarrior::ui::components {

class GLMenuButton : public GLComponentBase {
 public:
    explicit GLMenuButton(GLContext &glContext,
                          const std::string &title,
                          thewarrior::models::Point<float> location,
                          thewarrior::models::Size<float> size);
    ~GLMenuButton() override;
    bool isEnabled() const;
    void setCaption(const std::string &title);
    void setEnabled(bool enabled);
    void onGenerateGLElements() override;
    void onRender() override;
    void onGameWindowSizeChanged(const thewarrior::models::Size<int> &) override;
    void setTextureBeginId(int value);
    void setHasFocus(bool value);
    boost::signals2::signal<void()> onClickEvent;

 protected:
    GLTextObject m_glCaption;
    std::vector<GLObject> m_windowObjects;
    std::vector<GLObject> m_windowBackgrounds;
    int m_textureBeginId;
    bool m_hasFocus;
    bool m_enabled;
    void generateQuad(std::vector<GLObject> &objects,
                      thewarrior::models::Point<float> location,
                      thewarrior::models::Size<float> size,
                      const thewarrior::models::Texture *texture,
                      int textureId,
                      GLuint textureGLId = 0);
    void generateBoxQuad(std::vector<GLObject> &objects,
                         thewarrior::models::Point<float> location,
                         thewarrior::models::Size<float> size,
                         const thewarrior::models::Texture *texture,
                         int textureBeginId,
                         GLuint textureGLId = 0,
                         float blockSize = 32.0F);
    void generateCaption();
};

}  // namespace thewarrior::ui::components
