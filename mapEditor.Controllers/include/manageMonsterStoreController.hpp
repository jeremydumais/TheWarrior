#pragma once

#include "monsterStoreInfo.hpp"
#include <optional>
#include <string>
#include <vector>

namespace mapeditor::controllers {

struct CompareMonsterStoreName
{
public:
    CompareMonsterStoreName(const std::string &name);
    bool operator() (const MonsterStoreInfo &monsterStore);
private:
    std::string m_name;
};

class ManageMonsterStoreController
{
public:
    ManageMonsterStoreController(const std::string &resourcesPath,
                              const std::string &userConfigFolder,
                              const std::vector<MonsterStoreInfo> &monsterStores = {});
    const std::string &getLastError() const;
    const std::string &getResourcesPath() const;
    const std::string &getUserConfigFolder() const;
    const std::vector<MonsterStoreInfo> &getMonsterStores() const;
    std::optional<MonsterStoreInfo> findMonsterStore(const std::string &name) const;
    bool addMonsterStore(const MonsterStoreInfo &value);
    bool updateMonsterStore(const std::string &monsterStoreNameToEdit, const MonsterStoreInfo &newValue);
    bool deleteMonsterStore(const std::string &monsterStoreNameToDelete);
    bool loadMonsterStore();
    bool saveMonsterStore();
private:
    std::string m_lastError = "";
    std::string m_resourcesPath = "";
    std::string m_userConfigFolder = "";
    std::vector<MonsterStoreInfo> m_monsterStores;
    static const std::string MONSTERSTORES_PATH;
    bool validateName(const std::string &name, const std::string &field);
    bool validateFilename(const std::string &filename, const std::string &field);
    std::vector<MonsterStoreInfo>::iterator getMonsterInStoreIterator(const std::string &name);
    std::vector<MonsterStoreInfo>::const_iterator getMonsterInStoreIterator(const std::string &name) const;
    bool isNameAlreadyExists(const std::string &name) const;
};

} // namespace mapeditor::controllers
