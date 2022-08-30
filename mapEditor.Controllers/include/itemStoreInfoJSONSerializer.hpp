#pragma once

#include "itemStoreInfo.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <vector>

namespace mapeditor::controllers {

class ItemStoreInfoJSONSerializer
{
public:
    static boost::property_tree::ptree serialize(const std::vector<ItemStoreInfo> &itemStores);
    static std::vector<ItemStoreInfo> deserialize(boost::property_tree::ptree itemsNode);
};

} // namespace mapeditor::controllers
