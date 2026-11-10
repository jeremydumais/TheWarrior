#include <algorithm>
#include <string>
#include <vector>
#include "npcDialogueMessage.hpp"

namespace thewarrior::ui::models {

NPCDialogueMessage::NPCDialogueMessage(const std::string &message,
                                   int maxDurationInMilliseconds,
                                   float scale,
                                   const std::string &npcId,
                                   const std::vector<std::string> &dialogueLines)
: Message(dialogueLines.empty() ? message : "", maxDurationInMilliseconds, scale),
m_npcId(npcId),
m_dialogueLines(dialogueLines.empty() ? std::vector<std::string>{message} : dialogueLines) {
    setMessage(getPageText(3));
}

MessageType NPCDialogueMessage::getType() const {
    return MessageType::NPCDialogueMessage;
}

const std::string &NPCDialogueMessage::getNPCId() const {
    return m_npcId;
}

const std::vector<std::string> &NPCDialogueMessage::getDialogueLines() const {
    return m_dialogueLines;
}

size_t NPCDialogueMessage::getCurrentLineIndex() const {
    return m_currentLineIndex;
}

bool NPCDialogueMessage::hasNextPage(size_t linesPerPage) const {
    return m_currentLineIndex + linesPerPage < m_dialogueLines.size();
}

void NPCDialogueMessage::advancePage(size_t linesPerPage) {
    if (!hasNextPage(linesPerPage)) {
        return;
    }
    m_currentLineIndex += linesPerPage;
    setMessage(getPageText(linesPerPage));
    clearDisplayed();
}

void NPCDialogueMessage::setNPCId(const std::string &id) {
    m_npcId = id;
}

std::string NPCDialogueMessage::getPageText(size_t linesPerPage) const {
    const auto endIndex = std::min(m_currentLineIndex + linesPerPage, m_dialogueLines.size());
    std::string pageText;
    for (size_t index = m_currentLineIndex; index < endIndex; index++) {
        if (!pageText.empty()) {
            pageText += '\n';
        }
        pageText += m_dialogueLines[index];
    }
    return pageText;
}

}  // namespace thewarrior::ui::models
