#pragma once

#include "texture.hpp"
#include <GL/glew.h>

struct GLObject
{
    GLuint vao;
    GLuint vboPosition;
    GLuint vboColor;
    GLuint vboTexture;
};

struct GenerateGLObjectInfo {
    GLObject *glObject;
    const Texture *texture;
    const int textureIndex;
    GLuint *specialVAO = nullptr;
    GLuint *specialVBOTexture = nullptr;
};

class GLObjectService
{
public:
    static void generateGLObject(GenerateGLObjectInfo &info, const GLfloat tileCoord[4][2], const GLfloat colors[4][3]);
};