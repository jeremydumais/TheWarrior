#pragma once

#include "texture.hpp"
#include <GL/glew.h>

struct GenerateGLObjectInfo {
    const Texture *texture;
    GLuint *vao;
    const int textureIndex;
    GLuint *vboPosition;
    GLuint *vboColor;
    GLuint *vboTexture;
};

class GLObjectService
{
public:
    static void generateGLObject(GenerateGLObjectInfo &info, const GLfloat tileCoord[4][2], const GLfloat colors[4][3]);
};