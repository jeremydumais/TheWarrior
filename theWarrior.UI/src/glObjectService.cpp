#include "glObjectService.hpp"
#include "glTextureService.hpp"

void GLObjectService::generateGLObject(GenerateGLObjectInfo &info, const GLfloat tileCoord[4][2], const GLfloat colors[4][3]) 
{
    GLfloat texCoordBuf[4][2] { { 0.0F, 0.0F },
                                  { 0.0F, 0.0F }, 
                                  { 0.0F, 0.0F }, 
                                  { 0.0F, 0.0F } };
    GLTextureService textureService;
    if (info.texture != nullptr && info.textureIndex != -1) {
        textureService.setTextureUVFromIndex(info.texture, texCoordBuf, info.textureIndex);
    }
    GLuint *vaoPtr = info.specialVAO == nullptr ? &info.glObject->vao : info.specialVAO;
    glBindVertexArray(0);
    glGenVertexArrays(1, vaoPtr);
    glBindVertexArray(*vaoPtr);

    /* Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) */
    glGenBuffers(1, &info.glObject->vboPosition);
    glBindBuffer(GL_ARRAY_BUFFER, info.glObject->vboPosition);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tileCoord, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    /* Bind our second VBO as being the active buffer and storing vertex attributes (colors) */
    glGenBuffers(1, &info.glObject->vboColor);
    glBindBuffer(GL_ARRAY_BUFFER, info.glObject->vboColor);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    /* Bind our third VBO as being the active buffer and storing vertex attributes (textures) */
    GLuint *vboTexturePtr = info.specialVBOTexture == nullptr ? &info.glObject->vboTexture : info.specialVBOTexture;
    glGenBuffers(1, vboTexturePtr);
    glBindBuffer(GL_ARRAY_BUFFER, *vboTexturePtr);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), texCoordBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}