#pragma once

#include <map>
#include <memory>
#include <string>
#include "texture.hpp"

namespace thewarrior::ui {

struct GLContext {
    std::map<std::string, std::shared_ptr<thewarrior::models::Texture>>& textures;
    std::map<std::string, unsigned int>& texturesGL;
};

}  // namespace thewarrior::ui
