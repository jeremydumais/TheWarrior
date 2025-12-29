#pragma once

#include <string>
#include "messageDTO.hpp"

namespace thewarrior::ui::controllers {

struct ItemFoundMessageDTO : public MessageDTO {
    std::string itemId;
    std::string textureName;
    ~ItemFoundMessageDTO() override = default;
    MessageDTOType getType() override { return MessageDTOType::ItemFoundMessage; }
};

}  // namespace thewarrior::ui::controllers
