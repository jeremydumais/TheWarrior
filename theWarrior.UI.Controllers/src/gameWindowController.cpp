#include <fmt/format.h>
#include <GL/glew.h>
#include <libgen.h>         // dirname
#include <linux/limits.h>   // PATH_MAX
#include <unistd.h>         // readlink
#include <memory>
#include <string>
#include "gameWindowController.hpp"
#include "iGameStateRepository.hpp"
#include "saveGamePaths.hpp"
#include "sqliteGameStateRepository.hpp"

using thewarrior::storage::SQLiteGameStateRepository;

namespace thewarrior::ui::controllers {

GameWindowController::GameWindowController()
    : m_resourcesPath(""),
      m_lastError("") {
    initializeResourcesPath();
}

const std::string &GameWindowController::getResourcesPath() const {
    return m_resourcesPath;
}

const std::string &GameWindowController::getLastError() const {
    return m_lastError;
}

void GameWindowController::initializeResourcesPath() {
    std::string executablePath = "";
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        executablePath = dirname(result);
    }
    m_resourcesPath = fmt::format("{0}/resources/", executablePath);
}

bool GameWindowController::initializeSaveGameRepository() {
    std::unique_ptr<storage::IGameStateRepository> saveRepo =
        std::make_unique<SQLiteGameStateRepository>(storage::SaveGamePaths::getDatabaseFilePath());
    bool retval = saveRepo->initSchema();
    if (!retval) {
        m_lastError = saveRepo->getLastError();
    }
    return retval;
}

}  // namespace thewarrior::ui::controllers
