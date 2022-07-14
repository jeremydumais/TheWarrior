#pragma once

#include "message.hpp"
#include "messageType.hpp"
#include <string>

class ItemFoundMessage : public Message
{
public:
    ItemFoundMessage(const std::string &message, 
                     int maxDurationInMilliseconds,
                     const std::string &itemId,
                     const std::string &textureName);
    virtual ~ItemFoundMessage() = default;
    virtual MessageType getType() const override;
    const std::string &getItemId() const;
    const std::string &getTextureName() const;
    void setItemId(const std::string &id);
    void setTextureName(const std::string &name);
private:
    std::string m_itemId;
    std::string m_textureName;
};