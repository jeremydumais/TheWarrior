#pragma once

#include <string>
#include "message.hpp"
#include "messageType.hpp"

namespace thewarrior::ui::models {

class ItemFoundMessage : public Message {
 public:
    ItemFoundMessage(const std::string &message,
                     int maxDurationInMilliseconds,
                     float scale,
                     const std::string &itemId,
                     const std::string &textureName);
    ~ItemFoundMessage() override = default;
    MessageType getType() const override;
    const std::string &getItemId() const;
    const std::string &getTextureName() const;
    void setItemId(const std::string &id);
    void setTextureName(const std::string &name);

 private:
    std::string m_itemId;
    std::string m_textureName;
};

}  // namespace thewarrior::ui::models
