#pragma once

#include <memory>
#include <vector>
#include "glFormService.hpp"
#include "glObjectService.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "size.hpp"

namespace thewarrior::ui::components {

class GLComponentBase {
 public:
    GLComponentBase();
    virtual ~GLComponentBase() = default;
    void initialize(const std::shared_ptr<GLTexture> texture,
                    const std::shared_ptr<GLShaderProgram> shaderProgram,
                    std::shared_ptr<GLTextService> textService);
     void generateGLElements();
     void render();
     void gameWindowSizeChanged(const thewarrior::models::Size<int> &size);
 protected:
    thewarrior::models::Size<float> m_screenSize;
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
    std::shared_ptr<GLFormService> m_glFormService;
    std::shared_ptr<GLTextService> m_textService;
    std::shared_ptr<GLTexture> m_glTexture;
    static void freeGLObjects(std::vector<GLObject> &objects);
    virtual void onGenerateGLElements() {}
    virtual void onRender() {}
    virtual void onGameWindowSizeChanged(const thewarrior::models::Size<int> &) {}
};

}  // namespace thewarrior::ui::components
