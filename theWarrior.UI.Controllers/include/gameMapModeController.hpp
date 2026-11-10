#pragma once

#include <cstddef>
#include <memory>
#include <set>
#include <string>
#include <boost/signals2.hpp>
#include "itemDTO.hpp"
#include "itemStore.hpp"
#include "messageDTO.hpp"
#include "messagePipeline.hpp"
#include "monsterStore.hpp"
#include "player.hpp"
#include "point.hpp"
#include "story.hpp"
#include "worldState.hpp"

namespace thewarrior::ui::controllers {

class GameMapModeController {
 public:
    GameMapModeController();
    void initialize(const std::string &resourcesPath,
                    std::shared_ptr<thewarrior::models::WorldState> worldState,
                    const std::set<thewarrior::models::StoryId> &completedStoryIds);
    const std::string &getResourcesPath() const;
    const std::string &getCurrentMapName() const;
    const std::string &getLastError() const;
    std::shared_ptr<thewarrior::models::ItemStore> getItemStore();
    std::shared_ptr<thewarrior::models::MonsterStore> getMonsterStore();
    std::shared_ptr<thewarrior::models::WorldState> getWorldState();
    const std::set<thewarrior::models::StoryId> &getCompletedStoryIds() const;
    const thewarrior::models::Point<> &getPlayerPosition() const;
    ItemDTO findItem(const std::string &id) const;
    bool isMessageDisplayed() const;
    std::shared_ptr<MessageDTO> getCurrentMessage();
    size_t getMessageCount() const;
    std::shared_ptr<thewarrior::ui::models::MessagePipeline> getMessagePipeline();
    bool isTileActionAlreadyProcessed(const std::string &mapName, int tileIndex) const;
    bool isTileOccupyByNPC(const thewarrior::models::Point<> &position);
    bool isStoryIdCompleted(const thewarrior::models::StoryId &storyId) const;
    void setCurrentMapName(const std::string &mapName) const;
    void setPlayerPosition(const thewarrior::models::Point<> &position);
    void clearNPCsWorldState();
    void acknowledgeMessage();
    bool addItemToInventory(thewarrior::models::Player *player, const std::string &id);
    void awardGoldToPlayer(thewarrior::models::Player *player, unsigned int amount);
    void addMessageToPipeline(std::unique_ptr<MessageDTO> messageDTO);
    void deleteCurrentMessage();
    void displayCurrentMessage();
    bool currentNPCDialogueMessageHasNextPage() const;
    void advanceCurrentNPCDialogueMessagePage();
    void addTileActionProcessed(const std::string &mapName, int tileIndex);
    bool loadItemStore(const std::string &filePath);
    bool loadMonsterStore(const std::string &filePath);
    bool saveGameState(thewarrior::models::Player &player);
    void completeStory(const thewarrior::models::StoryId &storyId);
    boost::signals2::signal<void()> completedStoryIdsChanged;

 private:
    std::string m_lastError;
    std::string m_resourcesPath;
    std::shared_ptr<thewarrior::models::WorldState> m_worldState = nullptr;
    std::set<thewarrior::models::StoryId> m_completedStoryIds;
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    std::shared_ptr<thewarrior::models::MonsterStore> m_monsterStore;
    std::shared_ptr<thewarrior::ui::models::MessagePipeline> m_messagePipeline;
    static constexpr size_t NPCDialogueLinesPerPage = 3;
    std::shared_ptr<thewarrior::ui::models::Message> createMessageFromMessageDTO(std::unique_ptr<MessageDTO> dto) const;
    std::unique_ptr<MessageDTO> createMessageDTOFromMessage(std::shared_ptr<thewarrior::ui::models::Message> message) const;
};

}  // namespace thewarrior::ui::controllers
