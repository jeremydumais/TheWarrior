#include "monsterPickerFormController.hpp"
#include "monster.hpp"
#include "types.hpp"
#include <bits/ranges_algo.h>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iterator>
#include <memory>
#include <ranges>
#include <vector>

using namespace thewarrior::models;
using namespace boost::algorithm;

namespace mapeditor::controllers {

MonsterPickerFormController::MonsterPickerFormController(const std::shared_ptr<ContainerOfMonsterStore> monsterStores)
    : m_monsterStores(monsterStores)
{
}

std::vector<std::string> MonsterPickerFormController::getMonsterStoreNames() const
{
    std::vector<std::string> retval = {};
    std::transform(m_monsterStores->begin(),
                   m_monsterStores->end(),
                   std::back_inserter(retval),
                   [](std::pair<std::string, std::shared_ptr<MonsterStore>> element) {
                        return element.first;
                   });
    std::sort(retval.begin(), retval.end());
    return retval;
}

std::vector<MonsterCreationInfo> MonsterPickerFormController::getMonsters(const std::string &storeName,
        const std::string &filter) const
{
    std::vector<MonsterCreationInfo> retval = {};
    if (m_monsterStores != nullptr && m_monsterStores->contains(storeName)) {
        const auto store = m_monsterStores->find(storeName)->second;
        auto filterMonsters = [filter](const auto &monster) {
            return icontains(monster->getId(), filter) || icontains(monster->getName(), filter);
        };
        auto createMonsterInfo = [](const auto &monster) {
            return MonsterCreationInfo {
                monster->getId(),
                monster->getName(),
                monster->getTextureName(),
                monster->getTextureIndex(),
                monster->getHealth(),
                monster->getAttack(),
                monster->getDefense(),
                monster->getGoldRewardRange().first,
                monster->getGoldRewardRange().second
            };
        };
        auto monsters = store->getMonsters();
        std::ranges::transform(monsters | std::views::filter(filterMonsters),
                               std::back_inserter(retval),
                               createMonsterInfo);
    }
    return retval;
}

} // namespace mapeditor::controllers
