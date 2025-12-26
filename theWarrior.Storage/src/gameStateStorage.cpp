#include <fmt/format.h>
#include <memory>
#include <string>
#include <utility>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/serialization/export.hpp>
#include "gameStateStorage.hpp"
#include "binaryFileStream.hpp"
#include "gameState.hpp"

using boost::algorithm::trim_copy;

using namespace thewarrior::models;

namespace thewarrior::storage {

const std::string &GameStateStorage::getLastError() const {
    return m_lastError;
}

void GameStateStorage::loadGameState(const std::string &filename, GameState &gameState) {
    if (trim_copy(filename).empty()) {
        throw std::invalid_argument("The filename cannot be empty.");
    }
    if (m_bfs == nullptr) {
        m_bfs = std::make_unique<BinaryFileStream<GameState>>(filename);
    }
    if (!m_bfs->open(FileOpenMode::Read)) {
        throw std::runtime_error(fmt::format("Unable to open the gameState {0}", filename));
    }
    if (!m_bfs->readAllInto(gameState)) {
        throw std::runtime_error(fmt::format("Unable to read the content of the gameState {0}", filename));
    }
    if (!m_bfs->close()) {
        throw std::runtime_error(fmt::format("Unable to close the gameState file {0}", filename));
    }
}

void GameStateStorage::saveGameState(const std::string &filename, GameState &gameState) {
    if (m_bfs == nullptr) {
        m_bfs = std::make_unique<BinaryFileStream<GameState>>(filename);
    }
    if (!m_bfs->open(FileOpenMode::Write)) {
        throw std::runtime_error(fmt::format("Unable to open the gameState {0}", filename));
    }
    if (!m_bfs->write(gameState)) {
        throw std::runtime_error(fmt::format("Unable to write the content of the gameState {0}", filename));
    }
    if (!m_bfs->close()) {
        throw std::runtime_error(fmt::format("Unable to close the gameState file {0}", filename));
    }
}

bool GameStateStorage::deleteGameStates(const std::vector<std::string> &fileNames) {
    for (const auto &fileName : fileNames) {
        if (m_bfs == nullptr) {
            m_bfs = std::make_unique<BinaryFileStream<GameState>>(fileName);
        } else {
            m_bfs->setFileName(fileName);
        }
        if (!m_bfs->remove()) {
            m_lastError = m_bfs->getLastError();
            return false;
        }
    }
    return true;
}

void GameStateStorage::setFileStream(std::unique_ptr<IBinaryFileStream<GameState>> bfs) {
    m_bfs = std::move(bfs);
}

}  // namespace thewarrior::storage
