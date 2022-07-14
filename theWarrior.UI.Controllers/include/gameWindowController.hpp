#pragma once

#include "itemDTO.hpp"
#include "itemFoundMessage.hpp"
#include "itemFoundMessageDTO.hpp"
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
    std::shared_ptr<ItemStore> getItemStore();
    std::shared_ptr<MessagePipeline> getMessagePipeline();
    bool loadItemStore(const std::string &filePath);
    size_t getMessageCount() const;
private:
    std::shared_ptr<ItemStore> m_itemStore;
    std::shared_ptr<MessagePipeline> m_messagePipeline;
    std::string m_resourcesPath;
    std::string m_lastError; 
    void initializeResourcesPath();
};