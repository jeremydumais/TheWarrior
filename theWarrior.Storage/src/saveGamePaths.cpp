#include <filesystem>
#include <string>
#include "saveGamePaths.hpp"
#include "specialFolders.hpp"

using thewarrior::utils::SpecialFolders;
namespace fs = std::filesystem;

namespace thewarrior::storage {

std::string SaveGamePaths::getDatabaseFileName() {
    return "savegames.sqlite";
}

std::string SaveGamePaths::getDatabaseFilePath() {
    return fs::path(SpecialFolders::getSaveGameDirectory()) / getDatabaseFileName();
}

}  // namespace thewarrior::storage
