#pragma once

#include <string>

struct ItemCreationInfo 
{
    std::string id;
    std::string name;
    std::string textureName;
    int textureIndex;
};

class Item
{
public:
    explicit Item(const ItemCreationInfo &itemInfo);
    virtual ~Item() = default;
    Item(const Item &) = default;
    Item(Item &&) = default;
    Item &operator=(Item &) = default;
    Item &operator=(Item &&) = default;
    virtual bool equals(const Item &other) const;
    bool operator==(const Item &other) const;
    bool operator!=(const Item &other) const;
    const std::string &getId() const;
    const std::string &getName() const;
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    void setId(const std::string &id);
    void setName(const std::string &name);
    void setTextureName(const std::string &name);
    void setTextureIndex(int index);
private:
    std::string id;
    std::string name; 
    std::string textureName;
    int textureIndex; 
};