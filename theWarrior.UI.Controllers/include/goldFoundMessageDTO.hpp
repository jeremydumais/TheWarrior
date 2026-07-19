#pragma once

#include "messageDTO.hpp"

namespace thewarrior::ui::controllers {

struct GoldFoundMessageDTO : public MessageDTO {
    unsigned int goldAmount;
    ~GoldFoundMessageDTO() override = default;
    MessageDTOType getType() override { return MessageDTOType::GoldFoundMessage; }
};

}  // namespace thewarrior::ui::controllers
