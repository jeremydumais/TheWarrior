#include <string>
#include "goldFoundMessage.hpp"

namespace thewarrior::ui::models {

GoldFoundMessage::GoldFoundMessage(const std::string &message,
                                   int maxDurationInMilliseconds,
                                   float scale,
                                   unsigned int goldAmount)
: Message(message, maxDurationInMilliseconds, scale),
m_goldAmount(goldAmount) {}

MessageType GoldFoundMessage::getType() const {
    return MessageType::GoldFoundMessage;
}

unsigned int GoldFoundMessage::getGoldAmount() const {
    return m_goldAmount;
}

void GoldFoundMessage::setGoldAmount(unsigned int amount) {
    m_goldAmount = amount;
}

}  // namespace thewarrior::ui::models
