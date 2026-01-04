#include <fmt/format.h>
#include <qpixmap.h>
#include <algorithm>
#include <filesystem>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include "selectNPCTextureFormController.hpp"
#include "iTexturePixmapProvider.hpp"
#include "texture.hpp"
#include "textureUtils.hpp"

using commoneditor::ui::ITexturePixmapProvider;
using thewarrior::models::Texture;

namespace mapeditor::controllers {

SelectNPCTextureFormController::SelectNPCTextureFormController(const std::string &resourcesPath,
                                                               const std::vector<Texture> &textures,
                                                               ITexturePixmapProvider &pixmapProvider)
: m_resourcesPath(resourcesPath),
m_textures(textures),
m_pixmapProvider(pixmapProvider) {}

const std::string &SelectNPCTextureFormController::getResourcesPath() const {
    return m_resourcesPath;
}

const std::string &SelectNPCTextureFormController::getLastError() const {
    return m_lastError;
}

std::vector<std::string> SelectNPCTextureFormController::getTextureNames() const {
    std::vector<std::string> result;
    result.reserve(m_textures.size());

    std::ranges::transform(
        m_textures,
        std::back_inserter(result),
        [](const Texture &texture) {
            return texture.getName();
        });
    return result;
}

SelectNPCTextureFormController::AvailableNPCResult SelectNPCTextureFormController::getAvailableNPCs(const std::string &textureName) {
    // Get requested texture
    const auto iter = std::find_if(m_textures.begin(), m_textures.end(), [&textureName](const Texture &texture) {
        return texture.getName() == textureName;
    });
    if (iter == m_textures.end()) {
        m_lastError = fmt::format("The texture {0} could not be found.", textureName);
        return { false, {}};
    }

    // Check if the texture has a corresponding with and height
    if ((iter->getWidth() % (iter->getTileWidth() * 3)) != 0 ||
        (iter->getHeight() % (iter->getTileHeight() * 4)) != 0) {
        m_lastError = "The selected texture does not match the required NPC format. NPC textures must be 3 tiles wide and 4 tiles high.";
        return { false, {}};
    }
    // Load the texture file
    auto completeTexturePath = std::filesystem::path(m_resourcesPath) / "textures" / iter->getFilename();
    std::vector<AvailableNPC> npcResult {};
    const auto tilesPerRow = iter->getWidth() / iter->getTileWidth();
        for (int rowFirstTileIndex = 3 * tilesPerRow;
            rowFirstTileIndex <= ((iter->getHeight() / iter->getTileHeight()) * tilesPerRow);
            rowFirstTileIndex += (4 * tilesPerRow)) {
            for (int columnIndex = 1; columnIndex < tilesPerRow; columnIndex += 3) {
                std::shared_ptr<QPixmap> pixmap = m_pixmapProvider.loadPixmap(completeTexturePath);
                std::shared_ptr<QPixmap> iconPixmap = nullptr;

                const auto finalTileIndex = rowFirstTileIndex + columnIndex;
                // The base index is the first tile of the NPC (lower left)
                const auto npcBaseIndex = finalTileIndex - 1 - (tilesPerRow * 3);
                if (pixmap) {
                    iconPixmap = std::make_shared<QPixmap>(commoneditor::ui::TextureUtils::getTextureTileImageFromTexture(pixmap.get(), finalTileIndex, *iter));
                }
                npcResult.push_back({npcBaseIndex, iconPixmap});
           }
       }
    return { true, npcResult };
}

SelectNPCTextureFormController::NPCAnimation SelectNPCTextureFormController::getNPCAnimationTiles(const std::string &textureName,
                                                                                                  int baseNPCIndex) {
    SelectNPCTextureFormController::NPCAnimation result;
    // Get requested texture
    const auto iter = std::find_if(m_textures.begin(), m_textures.end(), [&textureName](const Texture &texture) {
        return texture.getName() == textureName;
    });
    if (iter == m_textures.end()) {
        m_lastError = fmt::format("The texture {0} could not be found.", textureName);
        return { false };
    }
    auto completeTexturePath = std::filesystem::path(m_resourcesPath) / "textures" / iter->getFilename();
    std::shared_ptr<QPixmap> pixmap = m_pixmapProvider.loadPixmap(completeTexturePath);
    const auto tilesPerRow = iter->getWidth() / iter->getTileWidth();
    result.walkUp = getNPCMovementsByBaseIndex(pixmap.get(), baseNPCIndex, *iter);
    result.walkRight = getNPCMovementsByBaseIndex(pixmap.get(), baseNPCIndex + tilesPerRow, *iter);
    result.walkLeft = getNPCMovementsByBaseIndex(pixmap.get(), baseNPCIndex + (tilesPerRow * 2), *iter);
    result.walkDown = getNPCMovementsByBaseIndex(pixmap.get(), baseNPCIndex + (tilesPerRow * 3), *iter);
    result.success = true;
    return result;
}

std::vector<QPixmap> SelectNPCTextureFormController::getNPCMovementsByBaseIndex(const QPixmap *sourcePixmap,
                                                                                int baseIndex,
                                                                                const Texture &texture) {
    std::vector<QPixmap> result {};
    for (int i = 0; i < 3; i++) {
        auto iconPixmap = commoneditor::ui::TextureUtils::getTextureTileImageFromTexture(sourcePixmap, baseIndex + i, texture);
        result.push_back(iconPixmap);
    }
    return result;
}

}  // namespace mapeditor::controllers
