#include "monsterStoreInfoJSONSerializer.hpp"

using boost::property_tree::ptree;

namespace mapeditor::controllers {

ptree MonsterStoreInfoJSONSerializer::serialize(const std::vector<MonsterStoreInfo> &monsterStores) {
    ptree retval;
    for (const auto &monsterStore : monsterStores) {
        ptree monsterStoreNode;
        monsterStoreNode.put("name", monsterStore.name);
        monsterStoreNode.put("filename", monsterStore.filename);
        retval.push_back(std::make_pair("", monsterStoreNode));
    }
    return retval;
}

std::vector<MonsterStoreInfo> MonsterStoreInfoJSONSerializer::deserialize(ptree monsterStoresNode) {
    std::vector<MonsterStoreInfo> retval;
    for (const auto& monsterStore : monsterStoresNode.get_child("")) {
        retval.push_back({
            .name = monsterStore.second.get<std::string>("name", ""),
            .filename = monsterStore.second.get<std::string>("filename", "")
        });
    }
    return retval;
}

}  // namespace mapeditor::controllers
