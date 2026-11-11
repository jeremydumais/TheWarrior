#pragma once

#include <memory>
#include <string>
#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>
#include "itemStore.hpp"


namespace thewarrior::models {

class GameData {
 public:
    const ItemStore &getItemStore() const;
    std::shared_ptr<ItemStore> getItemStoreForEdition();
    const std::string &getLastError() const;
    void setItemStore(const ItemStore &store);

 private:
    friend class boost::serialization::access;
    std::shared_ptr<ItemStore> m_itemStore = std::make_shared<ItemStore>();
    std::string m_lastError;

    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */) {
        ar & *m_itemStore;
    }
};

}  // namespace thewarrior::models


BOOST_CLASS_VERSION(thewarrior::models::GameData, 1)
