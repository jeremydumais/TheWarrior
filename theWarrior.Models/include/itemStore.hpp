#pragma once

#include "texture.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class ItemStore
{
public:

private:
    std::vector<Texture> textures;
    std::unordered_map<std::string, int> items;
};