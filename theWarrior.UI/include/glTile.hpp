#pragma once

#include <GL/glew.h>
#include "glObjectService.hpp"
#include "mapTile.hpp"

namespace thewarrior::ui {

struct GLTile {
    GLObject glMainObject;
    GLObject glSecondObject;
    GLuint vaoSecondObject = 0;
    GLuint vboSecondTextureObject = 0;
    int x = 0;
    int y = 0;
    thewarrior::models::MapTile tile;
};

}  // namespace thewarrior::ui
