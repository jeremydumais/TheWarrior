#include "gameMapModeController.hpp"
#include "itemFoundMessage.hpp"
#include "itemFoundMessageDTO.hpp"

using namespace thewarrior::ui::models;

namespace thewarrior::ui::controllers {

GameMapModeController::GameMapModeController()
    : m_itemStore(nullptr),
    m_messagePipeline(nullptr)
{
}

void GameMapModeController::initialize(std::shared_ptr<ItemStore> itemStore,
        std::shared_ptr<MessagePipeline> messagePipeline)

{
    m_itemStore = itemStore;
    m_messagePipeline = messagePipeline;
}

bool GameMapModeController::isMessageDisplayed() const
{
    auto currentMessage = m_messagePipeline->getCurrentMessage();
    return currentMessage != nullptr && currentMessage->isDisplayed();
}

void GameMapModeController::acknowledgeMessage()
{
    m_messagePipeline->deleteCurrentMessage();
}

ItemDTO GameMapModeController::findItem(const std::string &id) const
{
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

bool GameMapModeController::addItemToInventory(Player *player, const std::string &id)
{
    auto item = m_itemStore->findItem(id);
    if (player == nullptr || item == nullptr) {
        return false;
    }
    return player->getInventory()->addItem(item);
}

void GameMapModeController::addMessageToPipeline(std::unique_ptr<MessageDTO> messageDTO)
{
    auto message = createMessageFromMessageDTO(std::move(messageDTO));
    if (message != nullptr) {
        m_messagePipeline->addMessage(message);
    }
}

std::shared_ptr<MessageDTO> GameMapModeController::getCurrentMessage()
{
    return createMessageDTOFromMessage(m_messagePipeline->getCurrentMessage());
}

void GameMapModeController::deleteCurrentMessage()
{
    m_messagePipeline->deleteCurrentMessage();
}

void GameMapModeController::displayCurrentMessage()
{
    auto currentMessage = m_messagePipeline->getCurrentMessage();
    if (currentMessage) {
        currentMessage->setDisplayed(std::chrono::_V2::system_clock::now());
    }
}

std::shared_ptr<Message> GameMapModeController::createMessageFromMessageDTO(std::unique_ptr<MessageDTO> dto) const
{
    if (!dto) {
        return nullptr;
    }

    switch (dto->getType())
    {
        case MessageDTOType::Message:
            return std::make_shared<Message>(dto->message, dto->maxDurationInMilliseconds);
            break;
        case MessageDTOType::ItemFoundMessage:
            {
                ItemFoundMessageDTO *itemFoundMsgDTO = dynamic_cast<ItemFoundMessageDTO *>(dto.get());
                return std::make_shared<ItemFoundMessage>(itemFoundMsgDTO->message,
                        itemFoundMsgDTO->maxDurationInMilliseconds,
                        itemFoundMsgDTO->itemId,
                        itemFoundMsgDTO->textureName);
            }
            break;
        default:
            return nullptr;
            break;
    }
}

std::unique_ptr<MessageDTO> GameMapModeController::createMessageDTOFromMessage(std::shared_ptr<Message> message) const
{
    if (!message) {
        return nullptr;
    }
    std::unique_ptr<MessageDTO> retval = nullptr;
    switch (message->getType())
    {
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

bool GameMapModeController::isTileActionAlreadyProcessed(const std::string &mapName, int tileIndex) const
{
    if (const auto tilesProcessed = m_mapTileIndexActionAlreadyProcessed.find(mapName);
            tilesProcessed != m_mapTileIndexActionAlreadyProcessed.end()) {
        const auto &tileIndices = tilesProcessed->second;
        return std::find(begin(tileIndices), end(tileIndices), tileIndex) != tileIndices.end();
    }
    return false;
}

void GameMapModeController::addTileActionProcessed(const std::string &mapName, int tileIndex)
{
    if (m_mapTileIndexActionAlreadyProcessed.find(mapName) == m_mapTileIndexActionAlreadyProcessed.end()) {
        m_mapTileIndexActionAlreadyProcessed.insert({mapName, std::vector<int>()});
    }
    auto &tileIndices = m_mapTileIndexActionAlreadyProcessed.at(mapName);
    if (std::find(begin(tileIndices), end(tileIndices), tileIndex) == tileIndices.end()) {
        tileIndices.push_back(tileIndex);
    }
}

} // namespace thewarrior::ui::models
