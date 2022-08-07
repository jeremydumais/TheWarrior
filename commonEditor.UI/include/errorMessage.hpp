#pragma once

#include <string>

namespace commoneditor::ui {

class ErrorMessage
{
public:
    ErrorMessage() = delete;
    static void show(const std::string &message,
					 const std::string &internalError = "");
};

} // namespace commoneditor::ui
