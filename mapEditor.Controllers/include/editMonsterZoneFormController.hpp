#pragma once

#include "monsterStoreInfo.hpp"
#include <string>
#include <vector>

namespace mapeditor::controllers {

class EditMonsterZoneFormController
{
public:
    const std::string &getLastError() const;
private:
    std::string m_lastError = "";
};

} // namespace mapeditor::controllers
