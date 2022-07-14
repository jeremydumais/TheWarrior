#pragma once

#include "item.hpp"
#include "textureContainer.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/version.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class ItemStore
{
public:
    ItemStore();
    const std::string &getLastError() const;
    size_t getItemCount() const;
    std::vector<std::shared_ptr<Item>> getItems() const;
    const std::shared_ptr<const Item> findItem(const std::string &id) const;
    bool isItemExists(const std::string &id) const;
    bool addItem(std::shared_ptr<Item> item);
    bool replaceItem(const std::string oldId, std::shared_ptr<Item> item);
    bool removeItem(const std::string &id);
    const TextureContainer &getTextureContainer() const;
    TextureContainer &getTextureContainerForEdition();
private:
    friend class boost::serialization::access;
    std::string m_lastError;
    TextureContainer m_textureContainer;
    std::unordered_map<std::string, std::shared_ptr<Item>> m_items;
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & m_textureContainer;
        ar & m_items;
    }
};

BOOST_CLASS_VERSION(ItemStore, 0)

