#pragma once

#include <string>

namespace thewarrior::ui::controllers {

struct ItemDTO
{
    std::string id;
    std::string name;
    std::string textureName;
    int textureIndex;
    virtual ~ItemDTO() = default;
};

} // namespace thewarrior::ui::controllers
