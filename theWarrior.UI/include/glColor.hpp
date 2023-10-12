#pragma once

#include <string>
#include <glm/glm.hpp>

namespace thewarrior::ui {

enum GLColor {
    White,
    Gray,
    LightGray,
    Green,
    Red
};

glm::vec3 getVec3FromGLColor(GLColor color);
glm::vec3 getVec3FromRGBString(const std::string &value);

}  // namespace thewarrior::ui
