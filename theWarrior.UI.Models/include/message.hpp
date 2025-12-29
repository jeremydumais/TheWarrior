#pragma once

#include <chrono>
#include <optional>
#include <string>
#include "messageType.hpp"

namespace thewarrior::ui::models {

class Message {
 public:
    explicit Message(const std::string &message, int maxDurationInMilliseconds = -1, float scale = 0.6F);
    virtual ~Message() = default;
    virtual MessageType getType() const;
    const std::string &getMessage() const;
    int getMaxDurationInMilliseconds() const;
    float getScale() const;
    std::optional<std::chrono::_V2::system_clock::time_point> getDisplayStartTime() const;
    bool isDisplayed() const;
    bool hasMessageExpired(std::chrono::_V2::system_clock::time_point currentTime) const;
    void setMessage(const std::string &message);
    void setMaxDurationInMilliseconds(int value);
    void setScale(float value);
    void setDisplayed(std::chrono::_V2::system_clock::time_point startTime);

 private:
    std::string m_message;
    int m_maxDurationInMilliseconds;
    float m_scale;
    std::optional<std::chrono::_V2::system_clock::time_point> m_displayStartTime;
};

}  // namespace thewarrior::ui::models
