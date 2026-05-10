#pragma once

#include <string>

namespace commoneditor::ui {

class WarningMessage {
 public:
    WarningMessage() = delete;
    static void show(const std::string &message,
                     const std::string &internalWarning = "");
};

}  // namespace commoneditor::ui
