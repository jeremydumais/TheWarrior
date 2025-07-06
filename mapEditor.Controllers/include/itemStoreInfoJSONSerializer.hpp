#pragma once

#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include "itemStoreInfo.hpp"

namespace mapeditor::controllers {

class ItemStoreInfoJSONSerializer {
 public:
    static boost::property_tree::ptree serialize(const std::vector<ItemStoreInfo> &itemStores);
    static std::vector<ItemStoreInfo> deserialize(boost::property_tree::ptree itemsNode);
};

}  // namespace mapeditor::controllers
