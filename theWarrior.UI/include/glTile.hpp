#pragma once

#include <GL/glew.h>
#include "glObjectService.hpp"
#include "mapTile.hpp"

namespace thewarrior::ui {

struct GLTile {
    GLObject glMainObject;
    GLObject glSecondObject;
    GLuint vaoSecondObject;
    GLuint vboSecondTextureObject;
    int x;
    int y;
    thewarrior::models::MapTile tile;
};

}  // namespace thewarrior::ui
