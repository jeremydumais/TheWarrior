#pragma once

#include "glObjectService.hpp"
#include "mapTile.hpp"
#include <GL/glew.h>
#include <string>

namespace thewarrior::ui {

struct GLTile
{
    GLObject glObject;
    GLuint vaoObject;
    GLuint vboTextureObject;
    int x;
    int y;
    MapTile tile;
};

} // namespace thewarrior::ui
