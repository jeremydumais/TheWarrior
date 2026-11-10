#pragma once

#include <string>
#include "message.hpp"
#include "messageType.hpp"

namespace thewarrior::ui::models {

class GoldFoundMessage : public Message {
 public:
    GoldFoundMessage(const std::string &message,
                     int maxDurationInMilliseconds,
                     float scale,
                     unsigned int goldAmount);
    ~GoldFoundMessage() override = default;
    MessageType getType() const override;
    unsigned int getGoldAmount() const;
    void setGoldAmount(unsigned int amount);

 private:
    unsigned int m_goldAmount;
};

}  // namespace thewarrior::ui::models
