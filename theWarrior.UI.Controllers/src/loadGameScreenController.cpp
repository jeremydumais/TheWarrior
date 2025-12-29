#include <memory>
#include <string>
#include "loadGameScreenController.hpp"
#include "iGameStateRepository.hpp"
#include "saveGamePaths.hpp"
#include "sqliteGameStateRepository.hpp"

using namespace thewarrior::storage;

namespace thewarrior::ui::controllers {

LoadGameScreenController::LoadGameScreenController()
: m_lastError("") {}


const std::string &LoadGameScreenController::getLastError() const {
    return m_lastError;
}

GameStateRepositoryListResult LoadGameScreenController::getGameStateList() {
    const auto dbPath = storage::SaveGamePaths::getDatabaseFilePath();
    std::unique_ptr<storage::IGameStateRepository> repo = std::make_unique<SQLiteGameStateRepository>(dbPath);
    const auto gameStatesResult = repo->getAllGameStates();
    if (!gameStatesResult.success) {
        m_lastError = repo->getLastError();
    }
    return gameStatesResult;
}

}  // namespace thewarrior::ui::controllers


