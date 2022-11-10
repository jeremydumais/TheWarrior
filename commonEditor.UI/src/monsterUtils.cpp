#include "monsterUtils.hpp"
#include "textureUtils.hpp"
#include <fmt/format.h>
#include <string>

using namespace thewarrior::models;
using namespace std::literals;

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

std::string MonsterUtils::getEncounterRatioStr(MonsterEncounterRatio ratio)
{
    switch (ratio) {
        case MonsterEncounterRatio::LessThanNormal:
            return "Less than normal"s;
            break;
        case MonsterEncounterRatio::Normal:
            return "Normal"s;
            break;
        case MonsterEncounterRatio::Rare:
            return "Rare"s;
            break;
        default:
            return ""s;
    }
}

MonsterEncounterRatio MonsterUtils::getEncounterRatioFromString(const std::string &ratioStr)
{
    if (ratioStr == "Normal") {
        return MonsterEncounterRatio::Normal;
    }
    else if (ratioStr == "Less than normal") {
        return MonsterEncounterRatio::LessThanNormal;
    }
    else if (ratioStr == "Rare") {
        return MonsterEncounterRatio::Rare;
    }
    else {
        throw std::invalid_argument(fmt::format("Encounter ratio {} cannot be determined.", ratioStr));
    }
}

} // namespace commoneditor::ui
