#include "gameWindowController.hpp"
#include "itemStoreStorage.hpp"
#include <fmt/format.h>
#include <GL/glew.h>
#include <libgen.h>         // dirname
#include <linux/limits.h>   // PATH_MAX
#include <unistd.h>         // readlink

GameWindowController::GameWindowController()
    : m_itemStore(std::make_shared<ItemStore>()),
      m_messagePipeline(std::make_shared<MessagePipeline>()),
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

std::shared_ptr<ItemStore> GameWindowController::getItemStore()
{
    return m_itemStore;
}

std::shared_ptr<MessagePipeline> GameWindowController::getMessagePipeline()
{
    return m_messagePipeline;
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

size_t GameWindowController::getMessageCount() const
{
    return m_messagePipeline->getMessageCount();
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
