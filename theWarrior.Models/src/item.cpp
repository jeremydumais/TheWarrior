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
    : id(itemInfo.id),
      name(itemInfo.name),
      textureName(itemInfo.textureName),
      textureIndex(itemInfo.textureIndex)
{
    validateId(id);
    validateName(name);
    validateTextureName(textureName);
    validateTextureIndex(textureIndex);
}

bool Item::equals(const Item &other) const 
{
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
        return false;    
    }
    return this->id == other.id &&
           this->name == other.name &&
           this->textureName == other.textureName &&
           this->textureIndex == other.textureIndex;
}

bool Item::operator==(const Item &other) const
{
    return equals(other);
}

bool Item::operator!=(const Item &other) const
{
    return !(*this == other);
}

const std::string &Item::getId() const
{
    return id;
}

const std::string &Item::getName() const
{
    return name;
}

const std::string &Item::getTextureName() const
{
    return textureName;
}

int Item::getTextureIndex() const
{
    return textureIndex;
}

void Item::setId(const std::string &id) 
{
    validateId(id);
    this->id = id;
}

void Item::setName(const std::string &name) 
{
    validateName(name);
    this->name = name;
}

void Item::setTextureName(const std::string &name) 
{
    validateTextureName(name);
    this->textureName = name;
}

void Item::setTextureIndex(const int index) 
{
    validateTextureIndex(index);
    this->textureIndex = index;
}

void validateId(const std::string &id) 
{
    string sanitizedId = trim_copy(id);
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