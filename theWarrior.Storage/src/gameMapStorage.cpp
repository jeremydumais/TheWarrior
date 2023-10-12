#include "gameMapStorage.hpp"
#include <fmt/format.h>
#include <stdexcept>
#include "binaryFileStream.hpp"
#include "gameMap.hpp"
#include <boost/algorithm/string.hpp>

using boost::algorithm::trim_copy;
using thewarrior::models::GameMap;

namespace thewarrior::storage {

void GameMapStorage::loadMap(const std::string &fileName, std::shared_ptr<GameMap> map) {
    if (trim_copy(fileName).empty()) {
        throw std::invalid_argument("The filename cannot be empty!");
    }
    if (map == nullptr) {
        throw std::invalid_argument("The map cannot be null!");
    }
    if (m_bfs == nullptr) {
        m_bfs = make_unique<BinaryFileStream<GameMap>>(fileName);
    }
    if (!m_bfs->open(FileOpenMode::Read)) {
        throw std::runtime_error(fmt::format("Unable to open the map {0}", fileName));
    }
    if (!m_bfs->readAllInto(*map)) {
        throw std::runtime_error(fmt::format("Unable to read the content of the map {0}", fileName));
    }
    if (!m_bfs->close()) {
        throw std::runtime_error(fmt::format("Unable to close the map file {0}", fileName));
    }
}

void GameMapStorage::setFileStream(std::unique_ptr<IBinaryFileStream<GameMap>> bfs) {
    m_bfs = std::move(bfs);
}

}  // namespace thewarrior::storage
