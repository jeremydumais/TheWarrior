#pragma once

#include "texture.hpp"
#include <GL/glew.h>

namespace thewarrior::ui {

struct GLObject
{
    GLuint vao;
    GLuint vboPosition;
    GLuint vboColor;
    GLuint vboTexture;
    GLuint textureGLId = 0;
};

struct GenerateGLObjectInfo {
    GLObject *glObject;
    const thewarrior::models::Texture *texture;
    const int textureIndex;
    GLuint *specialVAO = nullptr;
    GLuint *specialVBOTexture = nullptr;
};

class GLObjectService
{
public:
    static void generateGLObject(GenerateGLObjectInfo &info, const GLfloat tileCoord[4][2], const GLfloat colors[4][3]);
};

} // namespace thewarrior::ui
