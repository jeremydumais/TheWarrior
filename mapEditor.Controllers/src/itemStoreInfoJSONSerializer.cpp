#include "itemStoreInfoJSONSerializer.hpp"

using namespace boost::property_tree;

namespace mapeditor::controllers {

ptree ItemStoreInfoJSONSerializer::serialize(const std::vector<ItemStoreInfo> &itemStores)
{
    ptree retval;
    for(const auto &item : itemStores) {
        ptree itemNode;
        itemNode.put("name", item.name);
        itemNode.put("filename", item.filename);
        retval.push_back(std::make_pair("", itemNode));
    }
     return retval;
}

std::vector<ItemStoreInfo> ItemStoreInfoJSONSerializer::deserialize(ptree itemsNode)
{
    std::vector<ItemStoreInfo> retval;
    for(const auto& item : itemsNode.get_child(""))
    {
        retval.push_back({
            .name = item.second.get<std::string>("name", ""),
            .filename = item.second.get<std::string>("filename", "")
        });
    }
    return retval;
}

} // namespace mapeditor::controllers
