#include <memory>
#include <string>
#include "gameData.hpp"

namespace thewarrior::models {

const ItemStore &GameData::getItemStore() const {
    return *m_itemStore;
}

std::shared_ptr<ItemStore> GameData::getItemStoreForEdition() {
    return m_itemStore;
}

const std::string &GameData::getLastError() const {
    return m_lastError;
}

    
void GameData::setItemStore(const ItemStore &store) {
    *m_itemStore = store;
}

}  // namespace thewarrior::models
