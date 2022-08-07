#pragma once

#include "messageDTO.hpp"
#include <string>

namespace thewarrior::ui::controllers {

struct ItemFoundMessageDTO : public MessageDTO
{
    std::string itemId;
    std::string textureName;
    virtual ~ItemFoundMessageDTO() = default;
    virtual MessageDTOType getType() { return MessageDTOType::ItemFoundMessage; };
};

} // namespace thewarrior::ui::controllers
