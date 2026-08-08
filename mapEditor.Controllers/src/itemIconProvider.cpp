#include <fmt/format.h>
#include <map>
#include <memory>
#include <qpixmap.h>
#include <ranges>
#include <string>
#include <vector>
#include "itemIconProvider.hpp"
#include "textureUtils.hpp"

using commoneditor::ui::TextureUtils;

namespace mapeditor::controllers {

std::map<std::string, QIcon> ItemIconProvider::getIconsFromItemIds(
    const std::shared_ptr<ContainerOfItemStore> &itemStores,
    const std::vector<std::string> &itemIds,
    const std::string &resourcesPath) {
    if (itemStores->empty()) {
        return {};
    }

    std::map<std::string, QIcon> retval;
    std::map<std::string, std::shared_ptr<QPixmap>> textures;
    const auto itemStore = itemStores->begin()->second;

    for (const auto &itemId : itemIds) {
        const auto &item = itemStore->findItem(itemId);
        if (item) {
            const auto &textureName = item->getTextureName();
            auto textureIter = std::ranges::find_if(textures,
                [textureName](const std::pair<std::string, std::shared_ptr<QPixmap>> &texturePixmap) {
                    return texturePixmap.first == textureName;
                });
            std::shared_ptr<QPixmap> pixmap = nullptr;
            auto texture = itemStore->getTextureContainer().getTextureByName(textureName);
            if (texture.has_value()) {
                if (textureIter == textures.end()) {
                    const auto completeTexturePath = fmt::format("{0}/textures/{1}", resourcesPath, texture->get().getFilename());
                    pixmap = std::make_shared<QPixmap>(QString(completeTexturePath.c_str()));
                    textures.insert({textureName, pixmap});
                } else {
                    pixmap = textureIter->second;
                }

                if (pixmap) {
                    auto iconPixmap = TextureUtils::getTextureTileImageFromTexture(
                        pixmap.get(), item->getTextureIndex(), texture.value());
                    retval.insert({itemId, QIcon(iconPixmap)});
                }
            }
        }
    }
    return retval;
}

}  // namespace mapeditor::controllers
