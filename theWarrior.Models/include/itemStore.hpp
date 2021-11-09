#pragma once

#include "item.hpp"
#include "textureContainer.hpp"
#include <boost/optional.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class ItemStore
{
public:
    ItemStore();
    const std::string &getLastError() const;
    size_t getTextureCount() const;
    size_t getItemCount() const;
    const std::vector<Texture> &getTextures() const;
    boost::optional<const Texture &> getTextureByName(const std::string &name) const;
    bool addTexture(const TextureInfo &textureInfo);
    bool replaceTexture(const std::string &name, const TextureInfo &textureInfo);
    bool removeTexture(const std::string &name);
    boost::optional<const Item &> findItem(const std::string &id) const;
    bool addItem(const Item &item);
    bool replaceItem(const std::string oldId, const Item &item);
    bool removeItem(const std::string &id);
private:
    std::string lastError;
    TextureContainer textureContainer;
    std::unordered_map<std::string, Item> items;
};

