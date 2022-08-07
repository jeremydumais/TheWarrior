#include "mainController.hpp"
#include "monsterStoreStorage.hpp"
#include "manageMonsterController.hpp"
#include "textureUtils.hpp"
#include <fmt/format.h>
#include <qpixmap.h>
#include <algorithm>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <set>
#include <ranges>

using namespace commoneditor::ui;

namespace monstereditor::controllers {

MainController::MainController()
    : m_monsterStore(std::make_shared<MonsterStore>()),
      m_lastError("")
{
}

TextureContainer &MainController::getTextureContainerForEdition()
{
    return m_monsterStore->getTextureContainerForEdition();
}

std::shared_ptr<MonsterStore> MainController::getMonsterStore()
{
    return m_monsterStore;
}

const std::string &MainController::getLastError() const
{
    return m_lastError;
}

bool MainController::openMonsterStore(const std::string &fileName) {
    MonsterStoreStorage storage;
    try {
        storage.loadMonsterStore(fileName, m_monsterStore);
        return true;
    }
    catch(const std::exception &err) {
        m_lastError = err.what();
    }
    return false;
}

bool MainController::saveMonsterStore(const std::string &fileName)
{
    MonsterStoreStorage storage;
    try {
        storage.saveMonsterStore(fileName, m_monsterStore);
        return true;
    }
    catch(const std::exception &err) {
        m_lastError = err.what();
    }
    return false;
}

std::vector<MonsterListDisplay> MainController::getMonsters() const
{
    std::vector<MonsterListDisplay> retval {};
    auto monsters = m_monsterStore->getMonsters();
    auto createMonsterListDisplay = [](const auto &monster) {
        auto [min, max] = monster->getGoldRewardRange();
        return MonsterListDisplay { .id = monster->getId(),
                                    .name = monster->getName(),
                                    .health = monster->getHealth(),
                                    .attack = monster->getAttack(),
                                    .defense = monster->getDefense(),
                                    .goldMinimum = min,
                                    .goldMaximum = max
                                  }; };
    std::ranges::transform(monsters,
                           std::back_inserter(retval),
                           createMonsterListDisplay);
    return retval;
}

std::map<std::string, QIcon> MainController::getIconsFromMonsterIds(std::vector<std::string> monsterIds,
                                                                 const std::string &resourcesPath) const
{
    std::map<std::string, QIcon> retval;
    std::map<std::string, std::shared_ptr<QPixmap>> textures;

    for(const auto &monsterId : monsterIds) {
        //Find the monster in the monster store
        const auto &monster = m_monsterStore->findMonster(monsterId);
        if (monster) {
            //Find the texture in the loaded pixmap collection
            const auto &textureName = monster->getTextureName();
            auto textureIter = std::find_if(textures.begin(),
                                            textures.end(),
                                            [textureName] (const std::pair<std::string, std::shared_ptr<QPixmap>> texturePixmap) {
                return texturePixmap.first == textureName;
            });
            std::shared_ptr<QPixmap> pixmap = nullptr;
             //Find the texture
            auto texture = m_monsterStore->getTextureContainer().getTextureByName(monster->getTextureName());
            if (texture.has_value()) {
                //If not found, load it
                if (textureIter == textures.end()) {
                    auto completeTexturePath = fmt::format("{0}/textures/{1}", resourcesPath, texture->get().getFilename());
                    pixmap = std::make_shared<QPixmap>(QString(completeTexturePath.c_str()));
                    textures.insert({textureName, pixmap});
                }
                else {
                    pixmap = textureIter->second;
                }

                if (pixmap) {
                    auto iconPixmap = TextureUtils::getTextureTileImageFromTexture(pixmap.get(),
                                                                                monster->getTextureIndex(),
                                                                                texture.value());
                    retval.insert({monsterId, QIcon(iconPixmap)});
                }
            }
        }
    }
    return retval;
}

bool MainController::deleteMonster(const std::string &id)
{
    ManageMonsterController monsterController(m_monsterStore);
    if (!monsterController.deleteMonster(id)) {
        m_lastError = monsterController.getLastError();
        return false;
    }
    return true;
}

} //namespace monstereditor::controllers
