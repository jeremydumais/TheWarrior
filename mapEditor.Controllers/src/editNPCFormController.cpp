#include <fmt/format.h>
#include <qlist.h>
#include <qpixmap.h>
#include <qregularexpression.h>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "editNPCFormController.hpp"
#include "iTexturePixmapProvider.hpp"
#include "npcDTO.hpp"
#include "npcDTOUtils.hpp"
#include "texture.hpp"
#include "textureUtils.hpp"

using thewarrior::models::Texture;
using commoneditor::ui::ITexturePixmapProvider;
using mapeditor::controllers::NPCDTO;

namespace mapeditor::controllers {

EditNPCFormController::EditNPCFormController(const std::string &resourcesPath,
                                             const std::vector<Texture> &textures,
                                             ITexturePixmapProvider &pixmapProvider,
                                             const std::optional<NPCDTO> selectedNPC,
                                             const std::vector<std::string> &alreadyUsedNPCIds)
: m_resourcesPath(resourcesPath),
m_textures(textures),
m_texturePixmapProvider(pixmapProvider),
m_selectedNPC(selectedNPC),
m_alreadyUsedNPCIds(alreadyUsedNPCIds) {}

const std::string &EditNPCFormController::getResourcesPath() const {
    return m_resourcesPath;
}

const std::string &EditNPCFormController::getLastError() const {
    return m_lastError;
}

bool EditNPCFormController::isEditMode() const {
    return m_selectedNPC.has_value();
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

bool EditNPCFormController::isNPCIdAlreadyUsed(const std::string &id) const {
    auto sanitizedId = boost::to_lower_copy(boost::trim_copy(id));
    bool idFound = std::find_if(m_alreadyUsedNPCIds.begin(),
                                m_alreadyUsedNPCIds.end(),
                                [&sanitizedId](const std::string &npcId) {
                                    return boost::to_lower_copy(boost::trim_copy(npcId)) == sanitizedId;
                                }) != m_alreadyUsedNPCIds.end();
    return (!m_selectedNPC.has_value() && idFound) ||
        (m_selectedNPC.has_value() && (boost::to_lower_copy(boost::trim_copy(m_selectedNPC->id)) != sanitizedId && idFound));
}

std::vector<std::string> EditNPCFormController::convertPlainTextToLines(const QString &text) {
    std::vector<std::string> result {};
    QStringList lines = text.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        result.push_back(line.trimmed().toStdString());
    }
    return result;
}

bool EditNPCFormController::isDTOValid(NPCDTO &dto) {
    const auto conversionResult = NPCDTOUtils::toNPC(dto);
    if (!conversionResult.success()) {
        m_lastError = conversionResult.errorMessage;
        return false;
    }
    return true;
}

}  // namespace mapeditor::controllers
