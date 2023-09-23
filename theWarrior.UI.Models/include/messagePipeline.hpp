#pragma once

#include "message.hpp"
#include <memory>
#include <string>
#include <queue>

namespace thewarrior::ui::models {

class MessagePipeline
{
public:
    MessagePipeline();
    size_t getMessageCount() const;
    std::shared_ptr<Message> getCurrentMessage();
    void addMessage(std::shared_ptr<Message> message);
    void deleteCurrentMessage();
private:
    std::queue<std::shared_ptr<Message>> m_messages;
};

} // namespace thewarrior::ui::models
