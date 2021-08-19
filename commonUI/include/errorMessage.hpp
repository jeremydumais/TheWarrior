#pragma once

#include <string>

class ErrorMessage
{
public:
    ErrorMessage() = delete;
    static void show(const std::string &message,
					 const std::string &internalError = "");
};