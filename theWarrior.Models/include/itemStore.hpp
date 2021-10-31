#pragma once

#include "item.hpp"
#include "texture.hpp"
#include "textureInfo.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class ItemStore
{
public:
    ItemStore();
    size_t getTextureCount() const;
    size_t getItemCount() const;
private:
    std::vector<Texture> textures;
    std::unordered_map<std::string, Item> items;
};