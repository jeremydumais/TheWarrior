#include "glColor.hpp"
#include <stdexcept>
#include <string>
#include <sstream>
#include <glm/fwd.hpp>

namespace thewarrior::ui {

glm::vec3 getVec3FromGLColor(GLColor color) {
    switch (color) {
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
    }
    return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 getVec3FromRGBString(const std::string &value) {
    if (value.size() != 7 || value[0] != '#') {
        throw std::invalid_argument("Invalid hexadecimal color string");
    }
    int r, g, b;
    std::stringstream(value.substr(1, 2)) >> std::hex >> r;
    std::stringstream(value.substr(3, 2)) >> std::hex >> g;
    std::stringstream(value.substr(5, 2)) >> std::hex >> b;

    glm::vec3 color(static_cast<float>(r) / 255.0f,
                    static_cast<float>(g) / 255.0f,
                    static_cast<float>(b) / 255.0f);
    return color;
}

}  // namespace thewarrior::ui
