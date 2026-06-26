#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "message.hpp"
#include "messageType.hpp"

namespace thewarrior::ui::models {

class NPCDialogueMessage : public Message {
 public:
    NPCDialogueMessage(const std::string &message,
                     int maxDurationInMilliseconds,
                     float scale,
                     const std::string &npcId,
                     const std::vector<std::string> &dialogueLines = {});
    ~NPCDialogueMessage() override = default;
    MessageType getType() const override;
    const std::string &getNPCId() const;
    const std::vector<std::string> &getDialogueLines() const;
    size_t getCurrentLineIndex() const;
    bool hasNextPage(size_t linesPerPage) const;
    void advancePage(size_t linesPerPage);
    void setNPCId(const std::string &id);

 private:
    std::string m_npcId;
    std::vector<std::string> m_dialogueLines;
    size_t m_currentLineIndex = 0;
    std::string getPageText(size_t linesPerPage) const;
};

}  // namespace thewarrior::ui::models
