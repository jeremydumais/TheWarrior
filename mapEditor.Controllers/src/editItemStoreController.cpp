#include "editItemStoreController.hpp"
#include "configurationManager.hpp"
#include <fmt/format.h>

using namespace thewarrior::storage;

namespace mapeditor::controllers {

EditItemStoreController::EditItemStoreController(const std::string &resourcesPath,
                                                 const std::string &userConfigFolder,
                                                 const std::vector<ItemStoreInfo> &itemStores)
    : m_resourcesPath(resourcesPath),
      m_userConfigFolder(userConfigFolder),
      m_itemStores(itemStores)
{
}

const std::string &EditItemStoreController::getLastError() const
{
    return m_lastError;
}

const std::string &EditItemStoreController::getResourcesPath() const
{
    return m_resourcesPath;
}

bool EditItemStoreController::saveItemStore()
{
    //TODO For edition, receive the current item store name to be edited
    //
    //TODO Check the the item store name is not already used
    //TODO Load the configuration file
    //TODO Save the itemstore
    ConfigurationManager configManager(m_userConfigFolder + "config.json");
    if (configManager.load())
    {
        //configManager.setStringValue(MainForm::THEME_PATH, "Dark");
        //setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
        //if (!configManager.save())
        //{
        //ErrorMessage::show("An error occurred while saving the configuration file.",
        //configManager.getLastError());
        //}
    }
    else
    {
        m_lastError = fmt::format("An error occurred while loading the configuration file. {0}",
                                  configManager.getLastError());
        return false;
    }
    return true;
}

} // namespace mapeditor::controllers
