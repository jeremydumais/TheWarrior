#include "item.hpp"
#include <boost/algorithm/string.hpp>
#include <stdexcept>

using namespace boost::algorithm;
using namespace std;

void validateId(const std::string &id);
void validateName(const std::string &name);
void validateTextureName(const std::string &textureName);
void validateTextureIndex(int textureIndex);

Item::Item(const ItemCreationInfo &itemInfo) 
    : m_id(itemInfo.id),
      m_name(itemInfo.name),
      m_textureName(itemInfo.textureName),
      m_textureIndex(itemInfo.textureIndex),
      m_optionalDescription(itemInfo.optionalDescription)
{
    validateId(m_id);
    validateName(m_name);
    validateTextureName(m_textureName);
    validateTextureIndex(m_textureIndex);
}

//Used only for Boost Serialization
Item::Item()
    : m_id("tmp999"),
      m_name("<temp item>"),
      m_textureName("tmp"),
      m_textureIndex(0),
      m_optionalDescription("")
{
}

bool Item::equals(const Item &other) const 
{
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
        return false;    
    }
    return this->m_id == other.m_id &&
           this->m_name == other.m_name &&
           this->m_textureName == other.m_textureName &&
           this->m_textureIndex == other.m_textureIndex && 
           this->m_optionalDescription == other.m_optionalDescription;
}

bool Item::operator==(const Item &other) const
{
    return equals(other);
}

bool Item::operator!=(const Item &other) const
{
    return !(*this == other);
}

ItemType Item::getType() const
{
    return ItemType::Item;
}

const std::string &Item::getId() const
{
    return m_id;
}

const std::string &Item::getName() const
{
    return m_name;
}

const std::string &Item::getTextureName() const
{
    return m_textureName;
}

int Item::getTextureIndex() const
{
    return m_textureIndex;
}

const std::string &Item::getOptionalDescription() const
{
    return m_optionalDescription;
}

void Item::setId(const std::string &id) 
{
    validateId(id);
    m_id = id;
}

void Item::setName(const std::string &name) 
{
    validateName(name);
    m_name = name;
}

void Item::setTextureName(const std::string &name) 
{
    validateTextureName(name);
    m_textureName = name;
}

void Item::setTextureIndex(const int index) 
{
    validateTextureIndex(index);
    m_textureIndex = index;
}

void Item::setOptionalDescription(const std::string &description)
{
    m_optionalDescription = description;
}

void validateId(const std::string &id) 
{
    string sanitizedId { trim_copy(id) };
    if (sanitizedId.empty()) {
        throw invalid_argument("id cannot be empty.");
    }
    if (sanitizedId.length() != 6) {
        throw invalid_argument("id must be 6 characters long.");
    }
}

void validateName(const std::string &name) 
{
    if (trim_copy(name).empty()) {
        throw invalid_argument("name cannot be empty.");
    }
}

void validateTextureName(const std::string &textureName)
{
    if (trim_copy(textureName).empty()) {
        throw invalid_argument("textureName cannot be empty.");
    }
}

void validateTextureIndex(const int textureIndex) 
{
    if (textureIndex < 0) {
        throw invalid_argument("textureIndex must be a positive number.");
    }
}
