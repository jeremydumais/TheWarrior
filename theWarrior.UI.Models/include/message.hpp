#pragma once

#include "messageType.hpp"
#include <chrono>
#include <optional>
#include <string>

class Message
{
public:
    Message(const std::string &message, int maxDurationInMilliseconds = -1);
    virtual ~Message() = default;
    virtual MessageType getType() const;
    const std::string &getMessage() const;
    int getMaxDurationInMilliseconds() const;
    std::optional<std::chrono::_V2::system_clock::time_point> getDisplayStartTime() const;
    bool isDisplayed() const;
    bool hasMessageExpired(std::chrono::_V2::system_clock::time_point currentTime) const;
    void setMessage(const std::string &message);
    void setMaxDurationInMilliseconds(int value);
    void setDisplayed(std::chrono::_V2::system_clock::time_point startTime);
private:
    std::string m_message;
    int m_maxDurationInMilliseconds;
    std::optional<std::chrono::_V2::system_clock::time_point> m_displayStartTime;
};