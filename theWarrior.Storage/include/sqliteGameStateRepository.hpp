#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include "iGameStateRepository.hpp"

namespace thewarrior::storage {

class SQLiteGameStateRepository : public IGameStateRepository {
 public:
     explicit SQLiteGameStateRepository(const std::string &dbPath);
     ~SQLiteGameStateRepository() override;
     std::vector<GameStateMetadata> listGameStates() override;
     void save(const models::GameState &gameState) override;
     void remove(int id) override;
 private:
     sqlite3 *m_db = nullptr;
     void initSchema();
};

}  // namespace thewarrior::storage
