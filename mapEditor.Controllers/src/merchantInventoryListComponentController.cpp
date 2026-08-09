#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <boost/variant/get.hpp>
#include "conversationScenario.hpp"
#include "merchantInventoryListComponentController.hpp"
#include "types.hpp"

namespace mapeditor::controllers {

MerchantInventoryListComponentController::MerchantInventoryListComponentController(GLComponentController *glComponentController)
: m_glComponentController(glComponentController) {
}

const std::string &MerchantInventoryListComponentController::getLastError() const {
    return m_lastError;
}

std::vector<MerchantInventoryDTO> MerchantInventoryListComponentController::getMerchantInventories() const {
    return m_glComponentController->getMerchantInventories();
}

std::optional<std::reference_wrapper<MerchantInventoryDTO>> MerchantInventoryListComponentController::getMerchantInventoryByName(const std::string &name) const {
    return m_glComponentController->getMerchantInventoryByName(name);
}

std::shared_ptr<ContainerOfItemStore> MerchantInventoryListComponentController::getItemStores() const {
    return m_itemStores;
}

std::vector<std::string> MerchantInventoryListComponentController::getAlreadyUsedMerchantInventoryNames() const {
    return m_glComponentController->getAlreadyUsedMerchantInventoryNames();
}

bool MerchantInventoryListComponentController::isMerchantInventoryUsed(const std::string &name) const {
    using thewarrior::models::ConversationAction;
    using thewarrior::models::ConversationScenario;
    using thewarrior::models::MerchantShopAction;
    using thewarrior::models::SellItemsAction;

    const auto scenarioUsesInventory = [&name](const ConversationScenario &scenario) {
        return std::ranges::any_of(scenario.getNodes(), [&name](const auto &node) {
            const auto *action = boost::get<ConversationAction>(&node.getContent());
            if (action == nullptr) {
                return false;
            }
            if (const auto *merchantShop = boost::get<MerchantShopAction>(action)) {
                return merchantShop->merchantInventoryName == name;
            }
            if (const auto *sellItems = boost::get<SellItemsAction>(action)) {
                return sellItems->merchantInventoryName == name;
            }
            return false;
        });
    };

    const auto map = m_glComponentController->getMap();
    if (std::ranges::any_of(map->getNPCs(), [&scenarioUsesInventory](const auto &npc) {
            return std::ranges::any_of(npc.getConversationScenarios(), scenarioUsesInventory);
        })) {
        return true;
    }

    for (const auto &row : map->getTiles()) {
        for (const auto &tile : row) {
            for (const auto &trigger : tile.getTriggers()) {
                const auto *scenario = trigger.getConversationScenario();
                if (scenario != nullptr && scenarioUsesInventory(*scenario)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void MerchantInventoryListComponentController::setItemStores(const std::shared_ptr<ContainerOfItemStore> &itemStores) {
    m_itemStores = itemStores;
}

}  // namespace mapeditor::controllers
