#pragma once

#include "messageDTO.hpp"
#include <string>

struct ItemFoundMessageDTO : public MessageDTO
{
    std::string itemId;
    std::string textureName;
    virtual ~ItemFoundMessageDTO() = default;
    virtual MessageDTOType getType() { return MessageDTOType::ItemFoundMessage; };
};