#include "gameWindowController.hpp"
#include "itemStoreStorage.hpp"
#include <fmt/format.h>
#include <GL/glew.h>
#include <libgen.h>         // dirname
#include <linux/limits.h>   // PATH_MAX
#include <unistd.h>         // readlink

GameWindowController::GameWindowController()
    : m_itemStore(std::make_shared<ItemStore>()),
      m_messagePipeline(MessagePipeline()),
      m_resourcesPath(""),
      m_lastError("")
{
    initializeResourcesPath();
}

const std::string &GameWindowController::getResourcesPath() const
{
    return m_resourcesPath;
}

const std::string &GameWindowController::getLastError() const
{
    return m_lastError;
}

const std::shared_ptr<ItemStore> GameWindowController::getItemStore() const
{
    return m_itemStore;
}

bool GameWindowController::loadItemStore(const std::string &filePath)
{
    ItemStoreStorage storage;
    try {
        storage.loadItemStore(filePath, m_itemStore);      
        return true;
    }
    catch(const std::exception &err) {
        m_lastError = err.what();
    }
    return false;
}

ItemDTO GameWindowController::findItem(const std::string &id) const
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

size_t GameWindowController::getMessageCount() const
{
    return m_messagePipeline.getMessageCount();
}

std::shared_ptr<MessageDTO> GameWindowController::getCurrentMessage()
{
    return createMessageDTOFromMessage(m_messagePipeline.getCurrentMessage());
}

void GameWindowController::addMessageToPipeline(std::unique_ptr<MessageDTO> messageDTO)
{
    auto message = createMessageFromMessageDTO(std::move(messageDTO));
    if (message != nullptr) {
        m_messagePipeline.addMessage(message);
    }
}

void GameWindowController::deleteCurrentMessage()
{
    m_messagePipeline.deleteCurrentMessage();
}

void GameWindowController::displayCurrentMessage()
{
    auto currentMessage = m_messagePipeline.getCurrentMessage();
    if (currentMessage) {
        currentMessage->setDisplayed(std::chrono::_V2::system_clock::now());
    }
}

void GameWindowController::initializeResourcesPath()
{
    std::string executablePath = "";
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        executablePath = dirname(result);
    }
    m_resourcesPath = fmt::format("{0}/resources/", executablePath);
}

std::shared_ptr<Message> GameWindowController::createMessageFromMessageDTO(std::unique_ptr<MessageDTO> dto) const
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

std::unique_ptr<MessageDTO> GameWindowController::createMessageDTOFromMessage(std::shared_ptr<Message> message) const
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