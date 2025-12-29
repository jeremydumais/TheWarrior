#pragma once

#include <string>
namespace thewarrior::storage {

class SaveGamePaths {
 public:
     static std::string getDatabaseFileName();
     static std::string getDatabaseFilePath();
};

}  // namespace thewarrior::storage
