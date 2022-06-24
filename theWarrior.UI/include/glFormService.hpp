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

class GLFormService
{
public:
    GLFormService();
    void initialize(std::shared_ptr<GLShaderProgram> shaderProgram,
                    std::shared_ptr<GLTextService> textService);
    void generateQuad(GLObject &object, Point<float> location, Size<float> size, const Texture *texture = nullptr, int textureId = 0);
    void generateBoxQuad(std::vector<GLObject>::iterator begin, 
                         Point<float> location, 
                         Size<float> size,
                         const Texture *texture,
                         int textureBeginId);
    void drawQuad(const GLObject &glObject, GLuint textureGLIndex, float transparency = 0.0F);
    void drawText(const GLTextObject &glTextObject, GLColor color = GLColor::White);
    void gameWindowSizeChanged(const Size<> &size);
private:
    Size<float> m_gameWindowSize;
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
    std::shared_ptr<GLTextService> m_textService;
};