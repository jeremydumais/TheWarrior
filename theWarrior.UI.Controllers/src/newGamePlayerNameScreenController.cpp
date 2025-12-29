#include <algorithm>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <memory>
#include <string>
#include "newGamePlayerNameScreenController.hpp"
#include "iGameStateRepository.hpp"
#include "saveGamePaths.hpp"
#include "sqliteGameStateRepository.hpp"

using namespace thewarrior::storage;

namespace thewarrior::ui::controllers {

NewGamePlayerNameScreenController::NewGamePlayerNameScreenController()
: m_lastError("") {}


const std::string &NewGamePlayerNameScreenController::getLastError() const {
    return m_lastError;
}

bool NewGamePlayerNameScreenController::hasAtLeastTwoAlphaAscii(std::string_view value) {
    int count = 0;
    for (char ch : value) {
        if (std::isalpha(ch)) {
            if (++count == 2) return true;
        }
    }
    return false;
}

NewGamePlayerNameScreenController::HasGameStateResult NewGamePlayerNameScreenController::hasGameStatesForPlayer(std::string_view playerName) {
    const auto dbPath = storage::SaveGamePaths::getDatabaseFilePath();
    std::unique_ptr<storage::IGameStateRepository> repo = std::make_unique<SQLiteGameStateRepository>(dbPath);
    const auto gameStatesResult = repo->getAllGameStates();
    if (!gameStatesResult.success) {
        m_lastError = repo->getLastError();
        return { false, false };
    }
    bool playerNameFound = std::find_if(gameStatesResult.gameStateList.begin(),
                                        gameStatesResult.gameStateList.end(),
                                        [playerName](const auto &gameState) {
        auto lowerTrimmedPlayerName = boost::trim_copy(boost::to_lower_copy(std::string(playerName)));
        auto lowerTrimmedGameStatePlayerName = boost::trim_copy(boost::to_lower_copy(gameState.playerName));
        return (lowerTrimmedPlayerName.compare(lowerTrimmedGameStatePlayerName) == 0);
    }) != gameStatesResult.gameStateList.end();
    return { true, playerNameFound };
}

}  // namespace thewarrior::ui::controllers


