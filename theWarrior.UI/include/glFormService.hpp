#pragma once

#include "glColor.hpp"
#include "glObjectService.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "point.hpp"
#include "size.hpp"
#include "texture.hpp"
#include <memory>
#include <vector>

namespace thewarrior::ui {

class GLFormService
{
public:
    GLFormService();
    void initialize(std::shared_ptr<GLShaderProgram> shaderProgram,
                    std::shared_ptr<GLTextService> textService);
    void generateQuad(std::vector<GLObject> &objects,
                      thewarrior::models::Point<float> location,
                      thewarrior::models::Size<float> size,
                      const thewarrior::models::Texture *texture = nullptr,
                      int textureId = 0,
                      GLuint textureGLId = 0);
    void generateBoxQuad(std::vector<GLObject> &objects,
                         thewarrior::models::Point<float> location,
                         thewarrior::models::Size<float> size,
                         const thewarrior::models::Texture *texture,
                         int textureBeginId,
                         GLuint textureGLId = 0);
    void drawQuad(const GLObject &glObject, GLuint textureGLIndex, float transparency = 0.0F);
    void drawText(const GLTextObject &glTextObject);
    void drawText(const GLTextObject &glTextObject, GLColor color);
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
private:
    thewarrior::models::Size<float> m_gameWindowSize;
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
    std::shared_ptr<GLTextService> m_textService;
};

} // namespace thewarrior::ui
