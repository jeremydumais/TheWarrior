#pragma once

#include "itemStore.hpp"
#include "messageDTO.hpp"
#include "messagePipeline.hpp"
#include <memory>
#include <string>

class GameWindowController
{
public: 
    GameWindowController();
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;
    bool loadItemStore(const std::string &filePath);
    size_t getMessageCount() const;
    std::shared_ptr<MessageDTO> getCurrentMessage();
    void addMessageToPipeline(std::unique_ptr<MessageDTO> messageDTO);
    void deleteCurrentMessage();
    void displayCurrentMessage();
private:
    std::shared_ptr<ItemStore> m_itemStore;
    MessagePipeline m_messagePipeline;
    std::string m_resourcesPath;
    std::string m_lastError; 
    void initializeResourcesPath();
    std::shared_ptr<Message> createMessageFromMessageDTO(std::unique_ptr<MessageDTO> dto) const;
    std::unique_ptr<MessageDTO> createMessageDTOFromMessage(std::shared_ptr<Message> message) const;
};