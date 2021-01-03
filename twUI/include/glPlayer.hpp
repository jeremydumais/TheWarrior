#pragma once

#include <GL/glew.h>
#include <string>

struct GLPlayer {
    int x;
    int y;
    std::string textureName;
    int textureIndex;
    GLuint vao;
    GLuint vboPosition;
    GLuint vboColor;
    GLuint vboTexture;
};
