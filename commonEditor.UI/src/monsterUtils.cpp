#include "monsterUtils.hpp"
#include "textureUtils.hpp"
#include "fmt/format.h"

using namespace thewarrior::models;

namespace commoneditor::ui {

std::map<std::string, QIcon> MonsterUtils::getIconsFromMonsterIds(const std::vector<std::string> &monsterIds,
                                                                  const std::shared_ptr<MonsterStore> monsterStore,
                                                                  const std::string &resourcesPath)
{
    std::map<std::string, QIcon> retval;
    std::map<std::string, std::shared_ptr<QPixmap>> textures;

    for(const auto &monsterId : monsterIds) {
        //Find the monster in the monster store
        const auto &monster = monsterStore->findMonster(monsterId);
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
            auto texture = monsterStore->getTextureContainer().getTextureByName(monster->getTextureName());
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

} // namespace commoneditor::ui
