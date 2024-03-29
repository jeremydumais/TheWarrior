#include "glFormService.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui {

GLFormService::GLFormService()
    : m_gameWindowSize(1.0F, 1.0F),
      m_shaderProgram(nullptr)
{
}

void GLFormService::initialize(std::shared_ptr<GLShaderProgram> shaderProgram,
                               std::shared_ptr<GLTextService> textService)
{
    m_shaderProgram = shaderProgram;
    m_textService = textService;
}

void GLFormService::generateQuad(std::vector<GLObject> &objects,
                                 Point<float> location,
                                 Size<float> size,
                                 const Texture *texture,
                                 int textureId,
                                 GLuint textureGLId)
{
    GLfloat texColorBuf[4][3];
    //Limits: -1.0F to 1.0F (top left corner: -1.0F, 1.0F - bottom right corner: 1.0F, -1.0F)
    auto invLocationX = location.x();
    auto invLocationY = location.y();
    auto gameWinW = m_gameWindowSize.width();
    auto gameWinH = m_gameWindowSize.height();
    const Point<float> normalizedLocation(((invLocationX - (gameWinW / 2.0F)) / gameWinW) * 2.0F,
                                          (((gameWinH - invLocationY) - (gameWinH / 2.0F)) / gameWinH) * 2.0F);

    const float BOXWIDTH = size.width() / gameWinW;
    const float BOXHEIGHT = size.height() / gameWinH;

    GLfloat tileCoord[4][2] = {
    { normalizedLocation.x(), normalizedLocation.y() }, /* Top Left point */
    { normalizedLocation.x() + (BOXWIDTH * 2.0F), normalizedLocation.y()  }, /* Top Right point */
    { normalizedLocation.x() + (BOXWIDTH * 2.0F), normalizedLocation.y() - (BOXHEIGHT * 2.0F)}, /* Bottom Right point */
    { normalizedLocation.x(), normalizedLocation.y() - (BOXHEIGHT * 2.0F)} }; /* Bottom Left point */

    objects.resize(objects.size()+1);
    GLObject &object = *objects.rbegin();
    object.textureGLId = textureGLId;
    if (!texture) {
        GenerateGLObjectInfo infoGenTexture {
            &object,
            nullptr,
            -1};
        GLObjectService::generateGLObject(infoGenTexture, tileCoord, texColorBuf);
    }
    else {
        GenerateGLObjectInfo infoGenTexture {
            &object,
            texture,
            textureId};
        GLObjectService::generateGLObject(infoGenTexture, tileCoord, texColorBuf);
    }
}

void GLFormService::generateBoxQuad(std::vector<GLObject> &objects,
                                    Point<float> location,
                                    Size<float> size,
                                    const Texture *texture,
                                    int textureBeginId,
                                    GLuint textureGLId)
{
    const float BLOCKSIZE = 32.0F;
    //Top left corner
    generateQuad(objects,
                 {location.x(), location.y()},
                 { BLOCKSIZE, BLOCKSIZE },
                 texture, textureBeginId, textureGLId);

    //Top horizontal middle
    generateQuad(objects,
                 {location.x() + BLOCKSIZE, location.y()},
                 { size.width() - (BLOCKSIZE * 2.0F), BLOCKSIZE },
                 texture, textureBeginId + 1, textureGLId);

    //Top right corner
    generateQuad(objects,
                 {location.x() + (size.width() - BLOCKSIZE), location.y()},
                 { BLOCKSIZE, BLOCKSIZE },
                 texture, textureBeginId + 2, textureGLId);

    //Left vertical middle
    generateQuad(objects,
                 {location.x(), location.y() + BLOCKSIZE},
                 { BLOCKSIZE, size.height() - (BLOCKSIZE * 2.0F) },
                 texture, textureBeginId + 3, textureGLId);

    //Right vertical middle
    generateQuad(objects,
                 {location.x() + (size.width() - BLOCKSIZE), location.y() + BLOCKSIZE},
                 { BLOCKSIZE, size.height() - (BLOCKSIZE * 2.0F) },
                 texture, textureBeginId + 4, textureGLId);

    //Bottom left corner
    generateQuad(objects,
                 {location.x(), location.y() + (size.height() - BLOCKSIZE)},
                 { BLOCKSIZE, BLOCKSIZE },
                 texture, textureBeginId + 5, textureGLId);

    //Bottom horizontal middle
    generateQuad(objects,
                 {location.x() + BLOCKSIZE, location.y() + (size.height() - BLOCKSIZE)},
                 { size.width() - (BLOCKSIZE * 2.0F), BLOCKSIZE },
                 texture, textureBeginId + 6, textureGLId);

    //Bottom right corner
    generateQuad(objects,
                 {location.x() + (size.width() - BLOCKSIZE), location.y() + (size.height() - BLOCKSIZE)},
                 { BLOCKSIZE, BLOCKSIZE },
                 texture, textureBeginId + 7, textureGLId);
}

void GLFormService::drawQuad(const GLObject &glObject, GLuint textureGLIndex, float transparency)
{
    m_shaderProgram->use();
    GLint uniformTransparency = glGetUniformLocation(m_shaderProgram->getShaderProgramID(), "transparency");
    glUniform1f(uniformTransparency, transparency);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureGLIndex);
    glBindVertexArray(glObject.vao);
    glBindBuffer(GL_ARRAY_BUFFER, glObject.vboPosition);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, glObject.vboColor);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, glObject.vboTexture);
    glEnableVertexAttribArray(2);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLFormService::drawText(const GLTextObject &glTextObject)
{
    drawText(glTextObject, glTextObject.color);
}

void GLFormService::drawText(const GLTextObject &glTextObject, GLColor color)
{
    m_textService->useShader();
    m_textService->renderText(glTextObject.text,
                              glTextObject.position.x(),
                              m_gameWindowSize.height() - glTextObject.position.y(),
                              glTextObject.scale,                               // Scale
                              getVec3FromGLColor(color));       // Color
}

void GLFormService::gameWindowSizeChanged(const Size<> &size)
{
    m_gameWindowSize.setSize(static_cast<float>(size.width()),
                             static_cast<float>(size.height()));
}

} // namespace thewarrior::ui
