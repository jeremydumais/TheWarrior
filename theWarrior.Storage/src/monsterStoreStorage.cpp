#include <fmt/format.h>
#include <exception>
#include <boost/algorithm/string/trim.hpp>
#include "monsterStore.hpp"
#include "monsterStoreStorage.hpp"
#include "binaryFileStream.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/serialization/export.hpp>

using thewarrior::models::MonsterStore;
using boost::algorithm::trim_copy;

namespace thewarrior::storage {

void MonsterStoreStorage::loadMonsterStore(const std::string &fileName,
        std::shared_ptr<MonsterStore> monsterStore) {
    if (trim_copy(fileName).empty()) {
        throw std::invalid_argument("The fileName cannot be empty.");
    }
    if (monsterStore == nullptr) {
        throw std::invalid_argument("The monsterStore cannot be null.");
    }
    if (m_bfs == nullptr) {
        m_bfs = make_unique<BinaryFileStream<MonsterStore>>(fileName);
    }
    if (!m_bfs->open(FileOpenMode::Read)) {
        throw std::runtime_error(fmt::format("Unable to open the monsterStore {0}", fileName));
    }
    if (!m_bfs->readAllInto(*monsterStore)) {
        throw std::runtime_error(fmt::format("Unable to read the content of the monsterStore {0}", fileName));
    }
    if (!m_bfs->close()) {
        throw std::runtime_error(fmt::format("Unable to close the monsterStore file {0}", fileName));
    }
}

void MonsterStoreStorage::saveMonsterStore(const std::string &fileName, std::shared_ptr<MonsterStore> monsterStore) {
    if (trim_copy(fileName).empty()) {
        throw std::invalid_argument("The fileName cannot be empty.");
    }
    if (monsterStore == nullptr) {
        throw std::invalid_argument("The monsterStore cannot be null.");
    }
    if (m_bfs == nullptr) {
        m_bfs = make_unique<BinaryFileStream<MonsterStore>>(fileName);
    }
    if (!m_bfs->open(FileOpenMode::Write)) {
        throw std::runtime_error(fmt::format("Unable to open the monsterStore {0}", fileName));
    }
    if (!m_bfs->write(*monsterStore)) {
        throw std::runtime_error(fmt::format("Unable to write the content of the monsterStore {0}", fileName));
    }
    if (!m_bfs->close()) {
        throw std::runtime_error(fmt::format("Unable to close the monsterStore file {0}", fileName));
    }
}

void MonsterStoreStorage::setFileStream(std::unique_ptr<IBinaryFileStream<MonsterStore>> bfs) {
    m_bfs = std::move(bfs);
}

}  // namespace thewarrior::storage
