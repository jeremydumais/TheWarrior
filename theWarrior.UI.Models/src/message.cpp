#include "message.hpp"

Message::Message(const std::string &message, int maxDurationInMilliseconds)
    : m_message(message),
      m_maxDurationInMilliseconds(maxDurationInMilliseconds),
      m_displayStartTime(std::nullopt)
{
}

MessageType Message::getType() const
{
    return MessageType::Message;
}
    
const std::string &Message::getMessage() const
{
    return m_message;
}

int Message::getMaxDurationInMilliseconds() const
{
    return m_maxDurationInMilliseconds;
}
    
std::optional<std::chrono::_V2::system_clock::time_point> Message::getDisplayStartTime() const
{
    return m_displayStartTime;
}

bool Message::isDisplayed() const
{
    return m_displayStartTime.has_value();
}

bool Message::hasMessageExpired(std::chrono::_V2::system_clock::time_point currentTime) const
{
    if (m_maxDurationInMilliseconds < 0 || !m_displayStartTime.has_value()) {
        return false;
    }
    auto expirationTime = m_displayStartTime.value() + std::chrono::milliseconds(m_maxDurationInMilliseconds);
    return currentTime >= expirationTime;  
}

void Message::setMessage(const std::string &message)
{
    m_message = message;
}

void Message::setMaxDurationInMilliseconds(int value)
{
    m_maxDurationInMilliseconds = value;
}

void Message::setDisplayed(std::chrono::_V2::system_clock::time_point startTime)
{
    m_displayStartTime = startTime;
}