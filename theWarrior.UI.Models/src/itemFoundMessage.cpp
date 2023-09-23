#include "itemFoundMessage.hpp"

namespace thewarrior::ui::models {

ItemFoundMessage::ItemFoundMessage(const std::string &message,
                                   int maxDurationInMilliseconds,
                                   const std::string &itemId,
                                   const std::string &textureName)
    : Message(message, maxDurationInMilliseconds),
      m_itemId(itemId),
      m_textureName(textureName)
{
}

MessageType ItemFoundMessage::getType() const
{
    return MessageType::ItemFoundMessage;
}

const std::string &ItemFoundMessage::getItemId() const
{
    return m_itemId;
}

const std::string &ItemFoundMessage::getTextureName() const
{
    return m_textureName;
}

void ItemFoundMessage::setItemId(const std::string &id)
{
    m_itemId = id;
}

void ItemFoundMessage::setTextureName(const std::string &name)
{
    m_textureName = name;
}

} // namespace thewarrior::ui::models
