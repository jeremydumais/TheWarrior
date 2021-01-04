#pragma once

#include <GL/glew.h>
#include <string>

struct GLPlayer {
    int x;
    int y;
    std::string textureName;
    GLuint vao;
    GLuint vboPosition;
    GLuint vboColor;
    GLuint vboTexture;
    float xMove;
    float yMove;
    int baseTextureIndex;
    int currentMovementTextureIndex;
};
