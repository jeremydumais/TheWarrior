#pragma once

#include <map>
#include <memory>
#include <qicon.h>
#include <string>
#include <vector>
#include "types.hpp"

namespace mapeditor::controllers {

class ItemIconProvider {
 public:
    static std::map<std::string, QIcon> getIconsFromItemIds(
        const std::shared_ptr<ContainerOfItemStore> &itemStores,
        const std::vector<std::string> &itemIds,
        const std::string &resourcesPath);
};

}  // namespace mapeditor::controllers
