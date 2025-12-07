#include <fmt/core.h>
#include <fmt/format.h>
#include <sqlite3.h>
#include <string>
#include <vector>
#include "sqliteGameStateRepository.hpp"
#include "gameStateMetadata.hpp"

namespace thewarrior::storage {

SQLiteGameStateRepository::SQLiteGameStateRepository(const std::string &dbPath)
: m_dbPath(dbPath) {}

SQLiteGameStateRepository::~SQLiteGameStateRepository() {}

const std::string &SQLiteGameStateRepository::getLastError() const {
    return m_lastError;
}

std::vector<GameStateMetadata> SQLiteGameStateRepository::listGameStates() {
}

bool SQLiteGameStateRepository::save(const GameStateMetadata &gameStateMetadata) {
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        m_lastError = fmt::format("Cannot open database: {0}", sqlite3_errmsg(m_db));
        return false;
    }

    const char* sql = "INSERT INTO GameStateMetadata (playerName, timestamp, level, fileName) VALUES (?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        m_lastError = fmt::format("Prepare failed: {0}", sqlite3_errmsg(m_db));
        sqlite3_close(m_db);
        return false;
    }

    // bind parameters
    sqlite3_bind_text(stmt, 1, gameStateMetadata.playerName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, gameStateMetadata.timestamp);
    sqlite3_bind_int(stmt, 3, static_cast<int>(gameStateMetadata.level));
    sqlite3_bind_text(stmt, 4, gameStateMetadata.fileName.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        m_lastError = fmt::format("Insert failed: {}", sqlite3_errmsg(m_db));
        sqlite3_finalize(stmt);
        sqlite3_close(m_db);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(m_db);
    return true;
}

void SQLiteGameStateRepository::remove(int id) {
}

bool SQLiteGameStateRepository::initSchema() {
    char *errorMessage = nullptr;
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        m_lastError = fmt::format("Cannot open database: {0}", sqlite3_errmsg(m_db));
        return false;
    }

    std::string createTableSQL =
        "CREATE TABLE IF NOT EXISTS GameStateMetadata ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "playerName TEXT NOT NULL,"
        "timestamp INTEGER NOT NULL,"
        "level INTEGER NOT NULL,"
        "fileName TEXT NOT NULL);";

    rc = sqlite3_exec(m_db, createTableSQL.c_str(), nullptr, nullptr, &errorMessage);
    if (rc != SQLITE_OK) {
        m_lastError = fmt::format("SQL error: {0}", errorMessage);
        sqlite3_free(errorMessage);
        sqlite3_close(m_db);
        return rc;
    }

    sqlite3_close(m_db);
    return true;
}

}  // namespace thewarrior::storage
