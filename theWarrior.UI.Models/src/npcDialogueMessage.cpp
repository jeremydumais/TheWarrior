#include "npcDialogueMessage.hpp"

namespace thewarrior::ui::models {

NPCDialogueMessage::NPCDialogueMessage(const std::string &message,
                                   int maxDurationInMilliseconds,
                                   float scale,
                                   const std::string &npcId)
: Message(message, maxDurationInMilliseconds, scale),
m_npcId(npcId) {}

MessageType NPCDialogueMessage::getType() const {
    return MessageType::NPCDialogueMessage;
}

const std::string &NPCDialogueMessage::getNPCId() const {
    return m_npcId;
}

void NPCDialogueMessage::setNPCId(const std::string &id) {
    m_npcId = id;
}

}  // namespace thewarrior::ui::models
