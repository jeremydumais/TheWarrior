#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "player.hpp"
#include "itemDTO.hpp"
#include "itemStore.hpp"
#include "messageDTO.hpp"
#include "messagePipeline.hpp"
#include "monsterStore.hpp"

namespace thewarrior::ui::controllers {

class GameMapModeController {
 public:
    GameMapModeController();
    void initialize(std::shared_ptr<thewarrior::models::ItemStore> itemStore,
            std::shared_ptr<thewarrior::models::MonsterStore> monsterStore,
            std::shared_ptr<thewarrior::ui::models::MessagePipeline> messagePipeline);
    bool isMessageDisplayed() const;
    void acknowledgeMessage();
    ItemDTO findItem(const std::string &id) const;
    bool addItemToInventory(thewarrior::models::Player *player, const std::string &id);
    void addMessageToPipeline(std::unique_ptr<MessageDTO> messageDTO);
    std::shared_ptr<MessageDTO> getCurrentMessage();
    void deleteCurrentMessage();
    void displayCurrentMessage();
    bool isTileActionAlreadyProcessed(const std::string &mapName, int tileIndex) const;
    void addTileActionProcessed(const std::string &mapName, int tileIndex);

 private:
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    std::shared_ptr<thewarrior::models::MonsterStore> m_monsterStore;
    std::shared_ptr<thewarrior::ui::models::MessagePipeline> m_messagePipeline;
    std::shared_ptr<thewarrior::ui::models::Message> createMessageFromMessageDTO(std::unique_ptr<MessageDTO> dto) const;
    std::unique_ptr<MessageDTO> createMessageDTOFromMessage(std::shared_ptr<thewarrior::ui::models::Message> message) const;
    std::map<std::string, std::vector<int>> m_mapTileIndexActionAlreadyProcessed;
};

}  // namespace thewarrior::ui::controllers
