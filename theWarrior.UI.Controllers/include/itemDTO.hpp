#pragma once

#include <string>

struct ItemDTO
{
    std::string id;
    std::string name;
    std::string textureName;
    int textureIndex;
    virtual ~ItemDTO() = default;
};