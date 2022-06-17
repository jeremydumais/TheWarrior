#pragma once

#include <glm/glm.hpp>

enum GLColor
{
    White,
    Gray
};

glm::vec3 getVec3FromGLColor(GLColor color);