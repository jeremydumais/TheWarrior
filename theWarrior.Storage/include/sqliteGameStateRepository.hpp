#pragma once

#include <sqlite3.h>
#include <string>
#include "gameStateMetadata.hpp"
#include "iGameStateRepository.hpp"

namespace thewarrior::storage {

constexpr const int SaveGameSlotMax = 5;

class SQLiteGameStateRepository : public IGameStateRepository {
 public:
     explicit SQLiteGameStateRepository(const std::string &dbPath);
     ~SQLiteGameStateRepository() override;
     const std::string &getLastError() const override;
     GameStateRepositoryListResult getAllGameStates() override;
     GameStateRepositorySaveResult save(const GameStateMetadata &gameStateMetadata) override;
     void remove(int id) override;
     bool initSchema() override;
 private:
     sqlite3 *m_db = nullptr;
     std::string m_dbPath = "";
     std::string m_lastError = "";
     bool insertGameStateRow(const GameStateMetadata &gameStateMetadata);
     GameStateRepositorySaveResult deleteObsoleteGameStates(const std::string &playerName);
};

}  // namespace thewarrior::storage
