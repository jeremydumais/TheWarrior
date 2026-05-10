#pragma once

#include <memory>
#include <string>
#include "itemStore.hpp"
#include "manageItemController.hpp"

namespace itemeditor::controllers {

struct StatsItemDTO : public ItemDTO {
    int statChangingIndex;
    float gain;
    bool limitOfOneApplied;
    unsigned int durationInSecs;
};

class ManageStatsItemController : public ManageItemController {
 public:
    explicit ManageStatsItemController(std::shared_ptr<thewarrior::models::ItemStore> itemStore);
    ~ManageStatsItemController() override = default;
    bool validateGain(const std::string &gainStr);
    bool validateDurationInSecs(const std::string &duration);
    std::unique_ptr<ItemDTO> getItem(const std::string &id) const override;

 private:
    std::shared_ptr<thewarrior::models::Item> itemDTOToItem(std::unique_ptr<ItemDTO> dto) override;
};

}  // namespace itemeditor::controllers
