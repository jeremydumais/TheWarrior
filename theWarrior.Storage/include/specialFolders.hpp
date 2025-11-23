#pragma once

#include <string>

namespace thewarrior::storage {

class SpecialFolders {
 public:
    static const std::string getAppConfigDirectory(const std::string &appConfigFolder);
    static const std::string getAppDataDirectory(const std::string &appDataFolder);
    static const std::string getSaveGameDirectory();
};

}  // namespace thewarrior::storage
