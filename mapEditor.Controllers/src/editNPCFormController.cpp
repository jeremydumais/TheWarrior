#include <fmt/format.h>
#include <qpixmap.h>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "editNPCFormController.hpp"
#include "iTexturePixmapProvider.hpp"
#include "texture.hpp"
#include "textureUtils.hpp"

using thewarrior::models::Texture;
using commoneditor::ui::ITexturePixmapProvider;

namespace mapeditor::controllers {

EditNPCFormController::EditNPCFormController(const std::string &resourcesPath,
                                             const std::vector<Texture> &textures,
                                             ITexturePixmapProvider &pixmapProvider)
: m_resourcesPath(resourcesPath),
m_textures(textures),
m_texturePixmapProvider(pixmapProvider) {}

const std::string &EditNPCFormController::getResourcesPath() const {
    return m_resourcesPath;
}

const std::string &EditNPCFormController::getLastError() const {
    return m_lastError;
}

const std::vector<thewarrior::models::Texture> &EditNPCFormController::getTextures() {
    return m_textures;
}

EditNPCFormController::NPCPixmapResult EditNPCFormController::getNPCPixmap(const std::string &textureName,
                                                                           const int baseTextureIndex) {
    // Get requested texture
    const auto iter = std::find_if(m_textures.begin(), m_textures.end(), [&textureName](const Texture &texture) {
        return texture.getName() == textureName;
    });
    if (iter == m_textures.end()) {
        m_lastError = fmt::format("The texture {0} could not be found.", textureName);
        return { false, nullptr};
    }
    // Load the texture file
    auto completeTexturePath = std::filesystem::path(m_resourcesPath) / "textures" / iter->getFilename();
    const auto tilesPerRow = iter->getWidth() / iter->getTileWidth();
    const auto frontFacingIndex = baseTextureIndex + (tilesPerRow * 3) + 1;
    std::shared_ptr<QPixmap> pixmap = m_texturePixmapProvider.loadPixmap(completeTexturePath);
    if (pixmap) {
        return {
            true,
            std::make_shared<QPixmap>(commoneditor::ui::TextureUtils::getTextureTileImageFromTexture(pixmap.get(), frontFacingIndex, *iter))
        };

    } else {
        m_lastError = fmt::format("Unable to load the texture {0}", textureName);
        return { false, nullptr };
    }
}

}  // namespace mapeditor::controllers
