#pragma once

#include <ctime>
#include <string>

namespace thewarrior::storage {

struct GameStateMetadata {
    int id;
    std::string playerName;
    std::time_t timestamp;
    unsigned int level;
    std::string fileName;
};

}  // namespace thewarrior::storage
