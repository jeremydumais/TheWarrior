#pragma once

#include "monsterStoreInfo.hpp"
#include "types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace mapeditor::controllers {

class EditMonsterZoneFormController
{
public:
    EditMonsterZoneFormController(const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores);
    const std::string &getLastError() const;
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> getMonsterStores();
private:
    std::string m_lastError = "";
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> m_monsterStores;
};

} // namespace mapeditor::controllers
