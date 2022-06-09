#pragma once

#include "itemDTO.hpp"
#include "itemStore.hpp"
#include "messageDTO.hpp"
#include "messagePipeline.hpp"
#include <memory>
#include <string>

class GameMapModeController
{
public:
    GameMapModeController();
    void initialize(std::shared_ptr<ItemStore> itemStore, 
                    std::shared_ptr<MessagePipeline> messagePipeline);
    bool isMessageDisplayed() const;
    void acknowledgeMessage();
    ItemDTO findItem(const std::string &id) const;
    void addMessageToPipeline(std::unique_ptr<MessageDTO> messageDTO);
    std::shared_ptr<MessageDTO> getCurrentMessage();
    void deleteCurrentMessage();
    void displayCurrentMessage();
private:
    std::shared_ptr<ItemStore> m_itemStore;
    std::shared_ptr<MessagePipeline> m_messagePipeline;
    std::shared_ptr<Message> createMessageFromMessageDTO(std::unique_ptr<MessageDTO> dto) const;
    std::unique_ptr<MessageDTO> createMessageDTOFromMessage(std::shared_ptr<Message> message) const;
};