#include "glColor.hpp"

namespace thewarrior::ui {

glm::vec3 getVec3FromGLColor(GLColor color)
{
    switch (color)
    {
    case GLColor::White:
        return glm::vec3(1.0f, 1.0f, 1.0f);
    case GLColor::Gray:
        return glm::vec3(0.3f, 0.3f, 0.3f);
    case GLColor::LightGray:
        return glm::vec3(0.6f, 0.6f, 0.6f);
    case GLColor::Green:
        return glm::vec3(0.0f, 1.0f, 0.0f);
    case GLColor::Red:
        return glm::vec3(1.0f, 0.0f, 0.0f);
    default:
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

} // namespace thewarrior::ui
