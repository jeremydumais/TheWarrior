#include <fmt/format.h>
#include <memory>
#include <string>
#include <utility>
#include "itemStoreStorage.hpp"
#include "armorItem.hpp"
#include "statsItem.hpp"
#include "weaponItem.hpp"
#include "binaryFileStream.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(thewarrior::models::WeaponItem)
BOOST_CLASS_EXPORT_IMPLEMENT(thewarrior::models::ArmorItem)
BOOST_CLASS_EXPORT_IMPLEMENT(thewarrior::models::StatsItem)

using namespace boost::algorithm;
using namespace thewarrior::models;
using namespace thewarrior::storage;

void ItemStoreStorage::loadItemStore(const std::string &fileName, std::shared_ptr<ItemStore> itemStore) {
    if (trim_copy(fileName).empty()) {
        throw std::invalid_argument("The fileName cannot be empty.");
    }
    if (itemStore == nullptr) {
        throw std::invalid_argument("The itemStore cannot be null.");
    }
    if (m_bfs == nullptr) {
        m_bfs = std::make_unique<BinaryFileStream<ItemStore>>(fileName);
    }
    if (!m_bfs->open(FileOpenMode::Read)) {
        throw std::runtime_error(fmt::format("Unable to open the itemStore {0}", fileName));
    }
    if (!m_bfs->readAllInto(*itemStore)) {
        throw std::runtime_error(fmt::format("Unable to read the content of the itemStore {0}", fileName));
    }
    if (!m_bfs->close()) {
        throw std::runtime_error(fmt::format("Unable to close the itemStore file {0}", fileName));
    }
}

void ItemStoreStorage::saveItemStore(const std::string &fileName, std::shared_ptr<ItemStore> itemStore) {
    if (trim_copy(fileName).empty()) {
        throw std::invalid_argument("The fileName cannot be empty.");
    }
    if (itemStore == nullptr) {
        throw std::invalid_argument("The itemStore cannot be null.");
    }
    if (m_bfs == nullptr) {
        m_bfs = std::make_unique<BinaryFileStream<ItemStore>>(fileName);
    }
    if (!m_bfs->open(FileOpenMode::Write)) {
        throw std::runtime_error(fmt::format("Unable to open the itemStore {0}", fileName));
    }
    if (!m_bfs->write(*itemStore)) {
        throw std::runtime_error(fmt::format("Unable to write the content of the itemStore {0}", fileName));
    }
    if (!m_bfs->close()) {
        throw std::runtime_error(fmt::format("Unable to close the itemStore file {0}", fileName));
    }
}

void ItemStoreStorage::setFileStream(std::unique_ptr<IBinaryFileStream<ItemStore>> bfs) {
    m_bfs = std::move(bfs);
}
