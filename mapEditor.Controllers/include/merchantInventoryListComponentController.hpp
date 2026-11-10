#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <boost/optional/optional.hpp>
#include "glComponentController.hpp"
#include "merchantInventoryDTO.hpp"
#include "types.hpp"

namespace mapeditor::controllers {

class MerchantInventoryListComponentController {
 public:
    explicit MerchantInventoryListComponentController(GLComponentController *glComponentController);
    const std::string &getLastError() const;
    
    std::vector<MerchantInventoryDTO> getMerchantInventories() const;
    std::optional<std::reference_wrapper<MerchantInventoryDTO>> getMerchantInventoryByName(const std::string &name) const;

    std::shared_ptr<ContainerOfItemStore> getItemStores() const;
    std::vector<std::string> getAlreadyUsedMerchantInventoryNames() const;
    bool isMerchantInventoryUsed(const std::string &name) const;

    void setItemStores(const std::shared_ptr<ContainerOfItemStore> &itemStores);
 private:
    GLComponentController *m_glComponentController = nullptr;
    std::string m_lastError;
    std::shared_ptr<mapeditor::controllers::ContainerOfItemStore> m_itemStores;
};

}  // namespace mapeditor::controllers
