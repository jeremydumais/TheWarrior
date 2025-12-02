#include <string>
#include <vector>
#include "sqliteGameStateRepository.hpp"

namespace thewarrior::storage {

SQLiteGameStateRepository::SQLiteGameStateRepository(const std::string &dbPath) {
}

SQLiteGameStateRepository::~SQLiteGameStateRepository() {}

std::vector<GameStateMetadata> SQLiteGameStateRepository::listGameStates() {
}

void SQLiteGameStateRepository::save(const models::GameState &gameState) {
}

void SQLiteGameStateRepository::remove(int id) {
}

}  // namespace thewarrior::storage
