#include <fmt/format.h>
#include <memory>
#include <string>
#include <utility>
#include "gameDataStorage.hpp"
#include "gameData.hpp"
#include "binaryFileStream.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/serialization/export.hpp>

using namespace boost::algorithm;
using namespace thewarrior::models;
using namespace thewarrior::storage;

void GameDataStorage::loadGameData(const std::string &fileName, std::shared_ptr<GameData> gameData) {
    if (trim_copy(fileName).empty()) {
        throw std::invalid_argument("The fileName cannot be empty.");
    }
    if (gameData == nullptr) {
        throw std::invalid_argument("The gameData cannot be null.");
    }
    if (m_bfs == nullptr) {
        m_bfs = std::make_unique<BinaryFileStream<GameData>>(fileName);
    }
    if (!m_bfs->open(FileOpenMode::Read)) {
        throw std::runtime_error(fmt::format("Unable to open the gameData {0}", fileName));
    }
    if (!m_bfs->readAllInto(*gameData)) {
        throw std::runtime_error(fmt::format("Unable to read the content of the gameData {0}", fileName));
    }
    if (!m_bfs->close()) {
        throw std::runtime_error(fmt::format("Unable to close the gameData file {0}", fileName));
    }
}

void GameDataStorage::saveGameData(const std::string &fileName, std::shared_ptr<GameData> gameData) {
    if (trim_copy(fileName).empty()) {
        throw std::invalid_argument("The fileName cannot be empty.");
    }
    if (gameData == nullptr) {
        throw std::invalid_argument("The gameData cannot be null.");
    }
    if (m_bfs == nullptr) {
        m_bfs = std::make_unique<BinaryFileStream<GameData>>(fileName);
    }
    if (!m_bfs->open(FileOpenMode::Write)) {
        throw std::runtime_error(fmt::format("Unable to open the gameData {0}", fileName));
    }
    if (!m_bfs->write(*gameData)) {
        throw std::runtime_error(fmt::format("Unable to write the content of the gameData {0}", fileName));
    }
    if (!m_bfs->close()) {
        throw std::runtime_error(fmt::format("Unable to close the gameData file {0}", fileName));
    }
}

void GameDataStorage::setFileStream(std::unique_ptr<IBinaryFileStream<GameData>> bfs) {
    m_bfs = std::move(bfs);
}
