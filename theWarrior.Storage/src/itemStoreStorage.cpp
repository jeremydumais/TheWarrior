#include "itemStoreStorage.hpp"
#include "armorItem.hpp"
#include "weaponItem.hpp"
#include "binaryFileStream.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/serialization/export.hpp>
#include <fmt/format.h>

BOOST_CLASS_EXPORT_GUID(WeaponItem, "WItem")
BOOST_CLASS_EXPORT_GUID(ArmorItem, "AItem")

using namespace std;
using namespace boost::algorithm;

void ItemStoreStorage::loadItemStore(const std::string &fileName, std::shared_ptr<ItemStore> itemStore) 
{
    if (trim_copy(fileName).empty()) {
        throw invalid_argument("The fileName cannot be empty.");
    }
    if (itemStore == nullptr) {
        throw invalid_argument("The itemStore cannot be null.");
    }
    if (m_bfs == nullptr) {
        m_bfs = make_unique<BinaryFileStream<ItemStore>>(fileName);
    }
    if (!m_bfs->open(FileOpenMode::Read)) {
		throw runtime_error(fmt::format("Unable to open the itemStore {0}", fileName));
	}
    if (!m_bfs->readAllInto(*itemStore)) {
		throw runtime_error(fmt::format("Unable to read the content of the itemStore {0}", fileName));
	}
    if (!m_bfs->close()) {
		throw runtime_error(fmt::format("Unable to close the itemStore file {0}", fileName));
	}
}

void ItemStoreStorage::saveItemStore(const std::string &fileName, std::shared_ptr<ItemStore> itemStore) 
{
    if (trim_copy(fileName).empty()) {
        throw invalid_argument("The fileName cannot be empty.");
    }
    if (itemStore == nullptr) {
        throw invalid_argument("The itemStore cannot be null.");
    }
    if (m_bfs == nullptr) {
        m_bfs = make_unique<BinaryFileStream<ItemStore>>(fileName);
    }
    if (!m_bfs->open(FileOpenMode::Write)) {
		throw runtime_error(fmt::format("Unable to open the itemStore {0}", fileName));
	}
    if (!m_bfs->write(*itemStore)) {
		throw runtime_error(fmt::format("Unable to write the content of the itemStore {0}", fileName));
	}
    if (!m_bfs->close()) {
		throw runtime_error(fmt::format("Unable to close the itemStore file {0}", fileName));
	}
}

void ItemStoreStorage::setFileStream(std::unique_ptr<IBinaryFileStream<ItemStore>> bfs) 
{
    m_bfs = move(bfs);
}
