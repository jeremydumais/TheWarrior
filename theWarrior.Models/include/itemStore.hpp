#pragma once

#include "item.hpp"
#include "textureContainer.hpp"
#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/version.hpp>
#include <optional>
#include <string>
#include <unordered_map>

class ItemStore
{
public:
    ItemStore();
    const std::string &getLastError() const;
    size_t getTextureCount() const;
    size_t getItemCount() const;
    const std::vector<Texture> &getTextures() const;
    std::optional<std::reference_wrapper<const Texture>> getTextureByName(const std::string &name) const;
    bool addTexture(const TextureInfo &textureInfo);
    bool replaceTexture(const std::string &name, const TextureInfo &textureInfo);
    bool removeTexture(const std::string &name);
    boost::optional<const Item &> findItem(const std::string &id) const;
    bool addItem(const Item &item);
    bool replaceItem(const std::string oldId, const Item &item);
    bool removeItem(const std::string &id);
private:
    friend class boost::serialization::access;
    std::string m_lastError;
    TextureContainer m_textureContainer;
    std::unordered_map<std::string, Item> m_items;
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & m_textureContainer;
        ar & m_items;
    }
};

BOOST_CLASS_VERSION(ItemStore, 0)
