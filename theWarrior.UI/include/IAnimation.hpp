#pragma once

#include <string>

namespace thewarrior::ui {

class IAnimation {
 public:
    virtual void process() = 0;
    virtual float getValue() = 0;
    virtual bool isCompleted() = 0;
    virtual ~IAnimation() = default;
};

}  // namespace thewarrior::ui

