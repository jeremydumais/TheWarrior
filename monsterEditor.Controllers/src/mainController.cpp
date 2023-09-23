#include "mainController.hpp"
#include "monsterStoreStorage.hpp"
#include "monsterUtils.hpp"
#include "manageMonsterController.hpp"
#include "textureUtils.hpp"
#include <fmt/format.h>
#include <qpixmap.h>
#include <algorithm>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <set>
#include <ranges>

using namespace commoneditor::ui;
using namespace thewarrior::models;
using namespace thewarrior::storage;

namespace monstereditor::controllers {

MainController::MainController()
    : m_monsterStore(std::make_shared<MonsterStore>()),
      m_lastError("")
{
}

TextureContainer &MainController::getTextureContainerForEdition()
{
    return m_monsterStore->getTextureContainerForEdition();
}

std::shared_ptr<MonsterStore> MainController::getMonsterStore()
{
    return m_monsterStore;
}

const std::string &MainController::getLastError() const
{
    return m_lastError;
}

bool MainController::openMonsterStore(const std::string &fileName) {
    MonsterStoreStorage storage;
    try {
        storage.loadMonsterStore(fileName, m_monsterStore);
        return true;
    }
    catch(const std::exception &err) {
        m_lastError = err.what();
    }
    return false;
}

bool MainController::saveMonsterStore(const std::string &fileName)
{
    MonsterStoreStorage storage;
    try {
        storage.saveMonsterStore(fileName, m_monsterStore);
        return true;
    }
    catch(const std::exception &err) {
        m_lastError = err.what();
    }
    return false;
}

std::vector<MonsterListDisplay> MainController::getMonsters() const
{
    std::vector<MonsterListDisplay> retval {};
    auto monsters = m_monsterStore->getMonsters();
    auto createMonsterListDisplay = [](const auto &monster) {
        auto [min, max] = monster->getGoldRewardRange();
        return MonsterListDisplay { .id = monster->getId(),
                                    .name = monster->getName(),
                                    .health = monster->getHealth(),
                                    .attack = monster->getAttack(),
                                    .defense = monster->getDefense(),
                                    .goldMinimum = min,
                                    .goldMaximum = max
                                  }; };
    std::ranges::transform(monsters,
                           std::back_inserter(retval),
                           createMonsterListDisplay);
    return retval;
}

std::map<std::string, QIcon> MainController::getIconsFromMonsterIds(std::vector<std::string> monsterIds,
                                                                    const std::string &resourcesPath) const
{
    return MonsterUtils::getIconsFromMonsterIds(monsterIds, m_monsterStore, resourcesPath);
}

bool MainController::deleteMonster(const std::string &id)
{
    ManageMonsterController monsterController(m_monsterStore);
    if (!monsterController.deleteMonster(id)) {
        m_lastError = monsterController.getLastError();
        return false;
    }
    return true;
}

} //namespace monstereditor::controllers
