#pragma once

#include <string>

namespace thewarrior::storage {

class SpecialFolders
{
public:
    static const std::string getAppConfigDirectory(const std::string &appName);
};

} // namespace thewarrior::storage
