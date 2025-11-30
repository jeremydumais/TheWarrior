#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "gameMapModeController.hpp"
#include "itemFoundMessage.hpp"
#include "itemFoundMessageDTO.hpp"
#include "itemStore.hpp"
#include "itemStoreStorage.hpp"
#include "monsterStoreStorage.hpp"

using namespace thewarrior::models;
using namespace thewarrior::ui::models;

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

bool GameMapModeController::isMessageDisplayed() const {
    auto currentMessage = m_messagePipeline->getCurrentMessage();
    return currentMessage != nullptr && currentMessage->isDisplayed();
}

const Point<> &GameMapModeController::getPlayerPosition() const {
    return m_worldState->getPlayerPosition();
}

void GameMapModeController::setCurrentMapName(const std::string &mapName) const {
    m_worldState->setCurrentMapName(mapName);
}

void GameMapModeController::setPlayerPosition(const Point<> &position) {
    m_worldState->setPlayerPosition(position);
}

void GameMapModeController::acknowledgeMessage() {
    m_messagePipeline->deleteCurrentMessage();
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

bool GameMapModeController::addItemToInventory(Player *player, const std::string &id) {
    auto item = m_itemStore->findItem(id);
    if (player == nullptr || item == nullptr) {
        return false;
    }
    return player->getInventory()->addItem(item);
}

std::shared_ptr<MessagePipeline> GameMapModeController::getMessagePipeline() {
    return m_messagePipeline;
}

size_t GameMapModeController::getMessageCount() const {
    return m_messagePipeline->getMessageCount();
}


void GameMapModeController::addMessageToPipeline(std::unique_ptr<MessageDTO> messageDTO) {
    auto message = createMessageFromMessageDTO(std::move(messageDTO));
    if (message != nullptr) {
        m_messagePipeline->addMessage(message);
    }
}

std::shared_ptr<MessageDTO> GameMapModeController::getCurrentMessage() {
    return createMessageDTOFromMessage(m_messagePipeline->getCurrentMessage());
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
            return std::make_shared<Message>(dto->message, dto->maxDurationInMilliseconds);
        case MessageDTOType::ItemFoundMessage:
            {
                ItemFoundMessageDTO *itemFoundMsgDTO = dynamic_cast<ItemFoundMessageDTO *>(dto.get());
                return std::make_shared<ItemFoundMessage>(itemFoundMsgDTO->message,
                        itemFoundMsgDTO->maxDurationInMilliseconds,
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
    retval->isDisplayed = message->isDisplayed();
    retval->isExpired = message->hasMessageExpired(std::chrono::_V2::system_clock::now());
    retval->maxDurationInMilliseconds = message->getMaxDurationInMilliseconds();
    return retval;
}

bool GameMapModeController::isTileActionAlreadyProcessed(const std::string &mapName, int tileIndex) const {
    return m_worldState->isTileActionAlreadyProcessed(mapName, tileIndex);
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


}  // namespace thewarrior::ui::controllers
