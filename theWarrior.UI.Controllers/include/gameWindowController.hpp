#pragma once

#include <memory>
#include <string>
#include "itemStore.hpp"
#include "messagePipeline.hpp"
#include "monsterStore.hpp"

namespace thewarrior::ui::controllers {

class GameWindowController {
 public:
    GameWindowController();
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;
    std::shared_ptr<thewarrior::models::ItemStore> getItemStore();
    std::shared_ptr<thewarrior::models::MonsterStore> getMonsterStore();
    std::shared_ptr<thewarrior::ui::models::MessagePipeline> getMessagePipeline();
    bool loadItemStore(const std::string &filePath);
    bool loadMonsterStore(const std::string &filePath);
    size_t getMessageCount() const;

 private:
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    std::shared_ptr<thewarrior::models::MonsterStore> m_monsterStore;
    std::shared_ptr<thewarrior::ui::models::MessagePipeline> m_messagePipeline;
    std::string m_resourcesPath;
    std::string m_lastError;
    void initializeResourcesPath();
};

}  // namespace thewarrior::ui::controllers
