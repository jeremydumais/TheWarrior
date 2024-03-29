#pragma once

#include <string>

namespace thewarrior::ui::controllers {

enum class MessageDTOType
{
    Message,
    ItemFoundMessage
};

struct MessageDTO
{
    std::string message;
    int maxDurationInMilliseconds;
    float scale = 0.6F;
    bool isDisplayed;
    bool isExpired;
    virtual ~MessageDTO() = default;
    virtual MessageDTOType getType() { return MessageDTOType::Message; };
};

} // namespace thewarrior::ui::controllers
