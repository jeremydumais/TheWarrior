#pragma once

#include <GL/glew.h>
#include <string>

struct GLTile
{
    GLuint vao;
    GLuint vaoObject;
    GLuint vboPosition;
    GLuint vboColor;
    GLuint vboTexture;
    GLuint vboTextureObject;
    bool hasTexture;
    std::string textureName;
    int textureIndex;
    bool hasObjectTexture;
    std::string objectTextureName;
    int objectTextureIndex;
};