#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include "gameStateMetadata.hpp"
#include "iGameStateRepository.hpp"

namespace thewarrior::storage {

class SQLiteGameStateRepository : public IGameStateRepository {
 public:
     explicit SQLiteGameStateRepository(const std::string &dbPath);
     ~SQLiteGameStateRepository() override;
     const std::string &getLastError() const override;
     std::vector<GameStateMetadata> listGameStates() override;
     bool save(const GameStateMetadata &gameStateMetadata) override;
     void remove(int id) override;
     bool initSchema() override;
 private:
     sqlite3 *m_db = nullptr;
     std::string m_dbPath = "";
     std::string m_lastError = "";
};

}  // namespace thewarrior::storage
