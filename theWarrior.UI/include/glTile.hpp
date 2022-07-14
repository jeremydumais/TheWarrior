#pragma once

#include "glObjectService.hpp"
#include "mapTile.hpp"
#include <GL/glew.h>
#include <string>

struct GLTile
{
    GLObject glObject;
    GLuint vaoObject;
    GLuint vboTextureObject;
    int x;
    int y;
    MapTile tile;
};