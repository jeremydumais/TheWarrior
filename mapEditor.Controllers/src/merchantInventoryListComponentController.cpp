#include <functional>
#include <memory>
#include <optional>
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

void MerchantInventoryListComponentController::setItemStores(const std::shared_ptr<ContainerOfItemStore> &itemStores) {
    m_itemStores = itemStores;
}

}  // namespace mapeditor::controllers
