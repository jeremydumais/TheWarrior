#include <cstddef>
#include <memory>
#include "messagePipeline.hpp"

namespace thewarrior::ui::models {

MessagePipeline::MessagePipeline()
    : m_messages(std::queue<std::shared_ptr<Message>>()) {
}

size_t MessagePipeline::getMessageCount() const {
    return m_messages.size();
}

std::shared_ptr<Message> MessagePipeline::getCurrentMessage() {
    if (m_messages.empty()) {
        return nullptr;
    }
    return m_messages.front();
}

void MessagePipeline::addMessage(std::shared_ptr<Message> message) {
    m_messages.push(message);
}

void MessagePipeline::deleteCurrentMessage() {
    m_messages.pop();
}

}  // namespace thewarrior::ui::models
