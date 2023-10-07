#pragma once

#include <string>
#include "itemType.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

namespace thewarrior::models {

struct ItemCreationInfo {
    std::string id;
    std::string name;
    std::string textureName;
    int textureIndex;
    std::string optionalDescription = "";
};

class Item {
 public:
    Item();  // Used only for Boost Serialization
    explicit Item(const ItemCreationInfo &itemInfo);
    virtual ~Item() = default;
    Item(const Item &) = default;
    Item(Item &&) = default;
    Item &operator=(const Item &) = default;
    Item &operator=(Item &&) = default;
    virtual bool equals(const Item &other) const;
    bool operator==(const Item &other) const;
    bool operator!=(const Item &other) const;
    virtual ItemType getType() const;
    const std::string &getId() const;
    const std::string &getName() const;
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    const std::string &getOptionalDescription() const;
    void setId(const std::string &id);
    void setName(const std::string &name);
    void setTextureName(const std::string &name);
    void setTextureIndex(int index);
    void setOptionalDescription(const std::string &description);

 protected:
    friend class boost::serialization::access;
    std::string m_id;
    std::string m_name;
    std::string m_textureName;
    int m_textureIndex;
    std::string m_optionalDescription;
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & m_id;
        ar & m_name;
        ar & m_textureName;
        ar & m_textureIndex;
        if (version == 1) {
            ar & m_optionalDescription;
        }
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::Item, 1)
