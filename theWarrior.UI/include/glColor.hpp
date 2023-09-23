#pragma once

#include <glm/glm.hpp>

namespace thewarrior::ui {

enum GLColor
{
    White,
    Gray,
    LightGray,
    Green,
    Red
};

glm::vec3 getVec3FromGLColor(GLColor color);

} // namespace thewarrior::ui
