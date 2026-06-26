#pragma once

#include <string>
#include "message.hpp"
#include "messageType.hpp"

namespace thewarrior::ui::models {

class NPCDialogueMessage : public Message {
 public:
    NPCDialogueMessage(const std::string &message,
                     int maxDurationInMilliseconds,
                     float scale,
                     const std::string &npcId);
    ~NPCDialogueMessage() override = default;
    MessageType getType() const override;
    const std::string &getNPCId() const;
    void setNPCId(const std::string &id);

 private:
    std::string m_npcId;
};

}  // namespace thewarrior::ui::models
