#include "glColor.hpp"

glm::vec3 getVec3FromGLColor(GLColor color)
{
    switch (color)
    {
    case GLColor::White:
        return glm::vec3(1.0f, 1.0f, 1.0f);
    case GLColor::Gray:
        return glm::vec3(0.3f, 0.3f, 0.3f);
    default:
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
}
