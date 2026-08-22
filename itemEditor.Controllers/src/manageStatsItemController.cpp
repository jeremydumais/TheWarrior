#include <limits>
#include <memory>
#include <string>
#include "manageStatsItemController.hpp"
#include "statsItem.hpp"
#include <boost/algorithm/string.hpp>

using namespace thewarrior::models;

namespace itemeditor::controllers {

ManageStatsItemController::ManageStatsItemController(std::shared_ptr<ItemStore> itemStore)
    : ManageItemController(itemStore) {
}

bool ManageStatsItemController::validateGain(const std::string &gainStr) {
    if (boost::trim_copy(gainStr).empty()) {
        m_lastError = "The gain value cannot be empty.";
        return false;
    }
    // Perform the str to float conversion
    try {
        std::stof(gainStr);
    } catch(const std::invalid_argument &) {
        m_lastError = "Unable to perform the float conversion of the gain.";
        return false;
    } catch(const std::out_of_range &) {
        m_lastError = "The gain value is out of range.";
        return false;
    }
    return true;
}

bool ManageStatsItemController::validateDurationInTurn(const std::string &duration) {
    if (boost::trim_copy(duration).empty()) {
        m_lastError = "The duration value cannot be empty.";
        return false;
    }
    // Perform the str to int conversion
    unsigned long durationUL = 0;
    try {
        durationUL = std::stoul(duration);
    } catch(const std::invalid_argument &) {
        m_lastError = "Unable to perform the unsigned int conversion of the duration.";
        return false;
    } catch(const std::out_of_range &) {
        m_lastError = "The duration value is out of range.";
        return false;
    }
    if (durationUL < std::numeric_limits<unsigned int>::min() ||
            durationUL > std::numeric_limits<unsigned int>::max()) {
        m_lastError = "The duration value is out of range.";
        return false;
    }
    return true;
}


std::unique_ptr<ItemDTO> ManageStatsItemController::getItem(const std::string &id) const {
    auto item = m_itemStore->findItem(id);
    if (item != nullptr) {
        const auto *statsItem = dynamic_cast<const StatsItem *>(item.get());
        if (statsItem != nullptr) {
            auto retval = std::make_unique<StatsItemDTO>();
            retval->id = statsItem->getId();
            retval->name = statsItem->getName();
            retval->textureName = statsItem->getTextureName();
            retval->textureIndex = statsItem->getTextureIndex();
            retval->optionalDescription = statsItem->getOptionalDescription();
            retval->defaultBuyPrice = statsItem->getDefaultBuyPrice();
            retval->defaultSellPrice = statsItem->getDefaultSellPrice();
            retval->statChangingIndex = static_cast<int>(statsItem->getStatChanging());
            retval->gain = statsItem->getGain();
            retval->limitOfOneApplied = statsItem->getLimitOfOneApplied();
            retval->durationInTurn = statsItem->getDurationInTurn();
            return retval;
        }
    }
    return nullptr;
}

std::shared_ptr<Item> ManageStatsItemController::itemDTOToItem(std::unique_ptr<ItemDTO> dto) {
    auto *statsDTO = dynamic_cast<StatsItemDTO *>(dto.get());
    if (statsDTO == nullptr) {
        return nullptr;
    }
    StatsItemCreationInfo creationInfo = {
        {
            statsDTO->id,
            statsDTO->name,
            statsDTO->textureName,
            statsDTO->textureIndex,
            statsDTO->optionalDescription,
            statsDTO->defaultBuyPrice,
            statsDTO->defaultSellPrice
        },
        static_cast<Stats>(statsDTO->statChangingIndex),
        statsDTO->gain,
        statsDTO->limitOfOneApplied,
        statsDTO->durationInTurn
    };
    std::shared_ptr<Item> updateItem = nullptr;
    try {
        updateItem = std::make_shared<StatsItem>(creationInfo);
    } catch(const std::invalid_argument &err) {
        m_lastError = err.what();
    }
    return updateItem;
}

}  // namespace itemeditor::controllers
