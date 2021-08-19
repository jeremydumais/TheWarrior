#pragma once

#include "mapTile.hpp"
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
    int x;
    int y;
    MapTile tile;
};