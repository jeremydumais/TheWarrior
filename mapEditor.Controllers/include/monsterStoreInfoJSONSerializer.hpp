#pragma once

#include "monsterStoreInfo.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <vector>

namespace mapeditor::controllers {

class MonsterStoreInfoJSONSerializer
{
public:
    static boost::property_tree::ptree serialize(const std::vector<MonsterStoreInfo> &monsterStores);
    static std::vector<MonsterStoreInfo> deserialize(boost::property_tree::ptree monsterStoresNode);
};

} // namespace mapeditor::controllers
