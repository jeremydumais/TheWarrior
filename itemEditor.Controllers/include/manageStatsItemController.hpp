#pragma once

#include "itemStore.hpp"
#include "manageItemController.hpp"
#include "statsItem.hpp"
#include <memory>
#include <string>

struct StatsItemDTO : public ItemDTO
{
    int statChangingIndex;
    float gain;
    bool limitOfOneApplied;
    unsigned int durationInSecs;
};

class ManageStatsItemController : public ManageItemController
{
public:
    ManageStatsItemController(std::shared_ptr<ItemStore> itemStore);
    virtual ~ManageStatsItemController() = default;
    bool validateGain(const std::string &gainStr); 
    bool validateDurationInSecs(const std::string &duration);
    virtual std::unique_ptr<ItemDTO> getItem(const std::string &id) const override;
private:
    virtual std::shared_ptr<Item> itemDTOToItem(std::unique_ptr<ItemDTO> dto) override;
};