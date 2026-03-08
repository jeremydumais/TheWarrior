#include <algorithm>
#include <fmt/chrono.h>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include "gameMapModeController.hpp"
#include "gameState.hpp"
#include "gameStateMetadata.hpp"
#include "gameStateStorage.hpp"
#include "iGameStateRepository.hpp"
#include "itemFoundMessage.hpp"
#include "itemFoundMessageDTO.hpp"
#include "itemStore.hpp"
#include "itemStoreStorage.hpp"
#include "monsterStoreStorage.hpp"
#include "saveGamePaths.hpp"
#include "specialFolders.hpp"
#include "sqliteGameStateRepository.hpp"

using namespace thewarrior::models;
using namespace thewarrior::storage;
using namespace thewarrior::ui::models;
namespace fs = std::filesystem;

namespace thewarrior::ui::controllers {

GameMapModeController::GameMapModeController()
    : m_itemStore(std::make_shared<ItemStore>()),
    m_monsterStore(std::make_shared<MonsterStore>()),
    m_messagePipeline(std::make_shared<MessagePipeline>()) {
}

void GameMapModeController::initialize(const std::string &resourcesPath,
                                       std::shared_ptr<thewarrior::models::WorldState> worldState) {
    m_resourcesPath = resourcesPath;
    m_worldState = worldState;
}

const std::string &GameMapModeController::getResourcesPath() const {
    return m_resourcesPath;
}

const std::string &GameMapModeController::getCurrentMapName() const {
    return m_worldState->getCurrentMapName();
}

const std::string &GameMapModeController::getLastError() const {
    return m_lastError;
}

std::shared_ptr<ItemStore> GameMapModeController::getItemStore() {
    return m_itemStore;
}

std::shared_ptr<MonsterStore> GameMapModeController::getMonsterStore() {
    return m_monsterStore;
}

std::shared_ptr<thewarrior::models::WorldState> GameMapModeController::getWorldState() {
    return m_worldState;
}

const Point<> &GameMapModeController::getPlayerPosition() const {
    return m_worldState->getPlayerPosition();
}

ItemDTO GameMapModeController::findItem(const std::string &id) const {
    ItemDTO dto;
    const auto item = m_itemStore->findItem(id);
    if (item) {
        dto.id = item->getId();
        dto.name = item->getName();
        dto.textureName = item->getTextureName();
        dto.textureIndex = item->getTextureIndex();
    }
    return dto;
}

bool GameMapModeController::isMessageDisplayed() const {
    auto currentMessage = m_messagePipeline->getCurrentMessage();
    return currentMessage != nullptr && currentMessage->isDisplayed();
}

std::shared_ptr<MessageDTO> GameMapModeController::getCurrentMessage() {
    return createMessageDTOFromMessage(m_messagePipeline->getCurrentMessage());
}

size_t GameMapModeController::getMessageCount() const {
    return m_messagePipeline->getMessageCount();
}

std::shared_ptr<MessagePipeline> GameMapModeController::getMessagePipeline() {
    return m_messagePipeline;
}

bool GameMapModeController::isTileActionAlreadyProcessed(const std::string &mapName, int tileIndex) const {
    return m_worldState->isTileActionAlreadyProcessed(mapName, tileIndex);
}

void GameMapModeController::setCurrentMapName(const std::string &mapName) const {
    m_worldState->setCurrentMapName(mapName);
}

void GameMapModeController::setPlayerPosition(const Point<> &position) {
    m_worldState->setPlayerPosition(position);
}

void GameMapModeController::clearNPCsWorldState() {
    m_worldState->clearNPCsState();
}

void GameMapModeController::acknowledgeMessage() {
    m_messagePipeline->deleteCurrentMessage();
}

bool GameMapModeController::addItemToInventory(Player *player, const std::string &id) {
    auto item = m_itemStore->findItem(id);
    if (player == nullptr || item == nullptr) {
        return false;
    }
    return player->getInventory()->addItem(item);
}

void GameMapModeController::addMessageToPipeline(std::unique_ptr<MessageDTO> messageDTO) {
    auto message = createMessageFromMessageDTO(std::move(messageDTO));
    if (message != nullptr) {
        m_messagePipeline->addMessage(message);
    }
}

void GameMapModeController::deleteCurrentMessage() {
    m_messagePipeline->deleteCurrentMessage();
}

void GameMapModeController::displayCurrentMessage() {
    auto currentMessage = m_messagePipeline->getCurrentMessage();
    if (currentMessage) {
        currentMessage->setDisplayed(std::chrono::_V2::system_clock::now());
    }
}

std::shared_ptr<Message> GameMapModeController::createMessageFromMessageDTO(std::unique_ptr<MessageDTO> dto) const {
    if (!dto) {
        return nullptr;
    }

    switch (dto->getType()) {
        case MessageDTOType::Message:
            return std::make_shared<Message>(dto->message, dto->maxDurationInMilliseconds, dto->scale);
        case MessageDTOType::ItemFoundMessage:
            {
                ItemFoundMessageDTO *itemFoundMsgDTO = dynamic_cast<ItemFoundMessageDTO *>(dto.get());
                return std::make_shared<ItemFoundMessage>(itemFoundMsgDTO->message,
                        itemFoundMsgDTO->maxDurationInMilliseconds,
                        itemFoundMsgDTO->scale,
                        itemFoundMsgDTO->itemId,
                        itemFoundMsgDTO->textureName);
            }
        default:
            return nullptr;
    }
}

std::unique_ptr<MessageDTO> GameMapModeController::createMessageDTOFromMessage(std::shared_ptr<Message> message) const {
    if (!message) {
        return nullptr;
    }
    std::unique_ptr<MessageDTO> retval = nullptr;
    switch (message->getType()) {
        case MessageType::Message:
            retval = std::make_unique<MessageDTO>();
            break;
        case MessageType::ItemFoundMessage:
            {
                retval = std::make_unique<ItemFoundMessageDTO>();
                ItemFoundMessage *itemFoundMessage = dynamic_cast<ItemFoundMessage *>(message.get());
                ItemFoundMessageDTO *dto = dynamic_cast<ItemFoundMessageDTO *>(retval.get());
                dto->itemId = itemFoundMessage->getItemId();
                dto->textureName = itemFoundMessage->getTextureName();
                break;
            }
        default:
            break;
    }
    retval->message = message->getMessage();
    retval->scale = message->getScale();
    retval->isDisplayed = message->isDisplayed();
    retval->isExpired = message->hasMessageExpired(std::chrono::_V2::system_clock::now());
    retval->maxDurationInMilliseconds = message->getMaxDurationInMilliseconds();
    return retval;
}

void GameMapModeController::addTileActionProcessed(const std::string &mapName, int tileIndex) {
    m_worldState->addTileActionProcessed(mapName, tileIndex);
}

bool GameMapModeController::loadItemStore(const std::string &filePath) {
    storage::ItemStoreStorage storage;
    try {
        storage.loadItemStore(filePath, m_itemStore);
        return true;
    }
    catch(const std::exception &err) {
        m_lastError = err.what();
    }
    return false;
}

bool GameMapModeController::loadMonsterStore(const std::string &filePath) {
    storage::MonsterStoreStorage storage;
    try {
        storage.loadMonsterStore(filePath, m_monsterStore);
        return true;
    }
    catch(const std::exception &err) {
        m_lastError = err.what();
    }
    return false;
}

bool GameMapModeController::saveGameState(thewarrior::models::Player &player) {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::string filename = fmt::format("{}_{:%Y-%m-%d-%H-%M-%S}.bak", player.getName(), tm);
    std::string fullPath = fs::path(utils::SpecialFolders::getSaveGameDirectory()) / filename;

    // Create the save game file
    GameStateStorage storage;
    GameState gameState(player, *m_worldState);
    try {
        storage.saveGameState(fullPath, gameState);
    } catch (const std::runtime_error &err) {
        m_lastError = err.what();
        return false;
    }
     // Convert the time point to a duration in seconds since the Unix epoch
    auto epoch_seconds = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch());
    // Get the integer count (the time_t equivalent)
    auto timestamp_int = epoch_seconds.count();
    GameStateMetadata metadata {
        .id = 0,
        .playerName = player.getName(),
        .timestamp = timestamp_int,
        .level = player.getLevel(),
        .fileName = filename
    };
    // Persist the save game metadata in the database
    std::unique_ptr<storage::IGameStateRepository> saveRepo =
        std::make_unique<SQLiteGameStateRepository>(storage::SaveGamePaths::getDatabaseFilePath());
    auto result = saveRepo->save(metadata);
    if (!result.success) {
        m_lastError = saveRepo->getLastError();
        return false;
    }
    // Delete obsolete game state files
    std::for_each(result.obsoleteFilePaths.begin(), result.obsoleteFilePaths.end(), [](auto &filePath) {
        filePath = fs::path(utils::SpecialFolders::getSaveGameDirectory()) / filePath;
    });
    if (!storage.deleteGameStates(result.obsoleteFilePaths)) {
        m_lastError = storage.getLastError();
        return false;
    }
    return true;
}


}  // namespace thewarrior::ui::controllers
