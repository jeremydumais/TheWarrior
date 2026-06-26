#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include "player.hpp"
#include "itemDTO.hpp"
#include "itemStore.hpp"
#include "messageDTO.hpp"
#include "messagePipeline.hpp"
#include "monsterStore.hpp"
#include "point.hpp"
#include "worldState.hpp"

namespace thewarrior::ui::controllers {

class GameMapModeController {
 public:
    GameMapModeController();
    void initialize(const std::string &resourcesPath,
                    std::shared_ptr<thewarrior::models::WorldState> worldState);
    const std::string &getResourcesPath() const;
    const std::string &getCurrentMapName() const;
    const std::string &getLastError() const;
    std::shared_ptr<thewarrior::models::ItemStore> getItemStore();
    std::shared_ptr<thewarrior::models::MonsterStore> getMonsterStore();
    std::shared_ptr<thewarrior::models::WorldState> getWorldState();
    const thewarrior::models::Point<> &getPlayerPosition() const;
    ItemDTO findItem(const std::string &id) const;
    bool isMessageDisplayed() const;
    std::shared_ptr<MessageDTO> getCurrentMessage();
    size_t getMessageCount() const;
    std::shared_ptr<thewarrior::ui::models::MessagePipeline> getMessagePipeline();
    bool isTileActionAlreadyProcessed(const std::string &mapName, int tileIndex) const;
    bool isTileOccupyByNPC(const thewarrior::models::Point<> &position);
    void setCurrentMapName(const std::string &mapName) const;
    void setPlayerPosition(const thewarrior::models::Point<> &position);
    void clearNPCsWorldState();
    void acknowledgeMessage();
    bool addItemToInventory(thewarrior::models::Player *player, const std::string &id);
    void addMessageToPipeline(std::unique_ptr<MessageDTO> messageDTO);
    void deleteCurrentMessage();
    void displayCurrentMessage();
    bool currentNPCDialogueMessageHasNextPage() const;
    void advanceCurrentNPCDialogueMessagePage();
    void addTileActionProcessed(const std::string &mapName, int tileIndex);
    bool loadItemStore(const std::string &filePath);
    bool loadMonsterStore(const std::string &filePath);
    bool saveGameState(thewarrior::models::Player &player);


 private:
    std::string m_lastError;
    std::string m_resourcesPath;
    std::shared_ptr<thewarrior::models::WorldState> m_worldState = nullptr;
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    std::shared_ptr<thewarrior::models::MonsterStore> m_monsterStore;
    std::shared_ptr<thewarrior::ui::models::MessagePipeline> m_messagePipeline;
    static constexpr size_t NPCDialogueLinesPerPage = 3;
    std::shared_ptr<thewarrior::ui::models::Message> createMessageFromMessageDTO(std::unique_ptr<MessageDTO> dto) const;
    std::unique_ptr<MessageDTO> createMessageDTOFromMessage(std::shared_ptr<thewarrior::ui::models::Message> message) const;
};

}  // namespace thewarrior::ui::controllers
