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
    return {};
}

SaveGameStateRepositoryResult SQLiteGameStateRepository::save(const GameStateMetadata &gameStateMetadata) {
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        m_lastError = fmt::format("Cannot open database: {0}", sqlite3_errmsg(m_db));
        return { false, {}};
    }
    sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    // Insert the new save game row
    if (!insertGameStateRow(gameStateMetadata)) {
        sqlite3_close(m_db);
        return {false, {}};
    }
    // Determine the obsolete save rows and return the save game files to remove
    auto saveGameStateRepositoryResult = deleteObsoleteGameStates(gameStateMetadata.playerName);
    if (!saveGameStateRepositoryResult.success) {
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        sqlite3_close(m_db);
        return {false, {}};
    }
    sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr);

    sqlite3_close(m_db);
    return saveGameStateRepositoryResult;
}

bool SQLiteGameStateRepository::insertGameStateRow(const GameStateMetadata &gameStateMetadata) {
    const char* sql = "INSERT INTO GameStateMetadata (playerName, timestamp, level, fileName) VALUES (?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);

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
    return true;
}

SaveGameStateRepositoryResult SQLiteGameStateRepository::deleteObsoleteGameStates(const std::string &playerName) {
    auto obsoleteGameStateIds = std::vector<int>();
    auto obsoleteFileNames = std::vector<std::string>();
    const char *selectSql =
        "SELECT id, fileName "
        "FROM GameStateMetadata "
        "WHERE playerName = ?"
        "ORDER BY timestamp DESC "
        "LIMIT -1 OFFSET ?;";  // skip the latest ? rows, return the rest

    sqlite3_stmt *selectStmt = nullptr;
    int rc = sqlite3_prepare_v2(m_db, selectSql, -1, &selectStmt, nullptr);
    if (rc != SQLITE_OK) {
        m_lastError = fmt::format("Failed to prepare select obsolete GameState metadata: {0}", sqlite3_errmsg(m_db));
        return {false, {}};
    }

    // Bind parameters
    sqlite3_bind_text(selectStmt, 1, playerName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(selectStmt, 2, SaveGameSlotMax);

    while ((rc = sqlite3_step(selectStmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(selectStmt, 0);
        obsoleteGameStateIds.push_back(id);
        const unsigned char *fileNameText = sqlite3_column_text(selectStmt, 1);
        std::string fileName;

        if (fileNameText != nullptr) {
            fileName = reinterpret_cast<const char *>(fileNameText);
        }
        obsoleteFileNames.push_back(fileName);
    }

    if (rc != SQLITE_DONE) {
        m_lastError = fmt::format("Error while selecting obsolete GameState metadata: {0}", sqlite3_errmsg(m_db));
        sqlite3_finalize(selectStmt);
        return {false, {}};
    }

    sqlite3_finalize(selectStmt);

    // If there are no obsolete rows, just return
    if (obsoleteFileNames.empty()) {
        return {true, {}};
    }

    // Step 2: delete those rows
    const char *deleteSql = "DELETE FROM GameStateMetadata WHERE id = ?;";

    sqlite3_stmt *deleteStmt = nullptr;
    rc = sqlite3_prepare_v2(m_db, deleteSql, -1, &deleteStmt, nullptr);
    if (rc != SQLITE_OK) {
        m_lastError = fmt::format("Failed to prepare delete obsolete GameState metadata: {0}", sqlite3_errmsg(m_db));
        return {false, {}};
    }

    for (const auto &id : obsoleteGameStateIds) {
        sqlite3_reset(deleteStmt);
        sqlite3_clear_bindings(deleteStmt);
        sqlite3_bind_int(deleteStmt, 1, id);

        rc = sqlite3_step(deleteStmt);
        if (rc != SQLITE_DONE) {
            m_lastError = fmt::format("Failed to delete obsolete GameState with id {0}: {1}", id, sqlite3_errmsg(m_db));
            sqlite3_finalize(deleteStmt);
            return {false, {}};
        }
    }

    sqlite3_finalize(deleteStmt);
    return {true, obsoleteFileNames};
}

void SQLiteGameStateRepository::remove(int) {
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
