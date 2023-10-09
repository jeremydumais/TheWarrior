#include "editTextureFormController.hpp"
#include <fmt/format.h>
#include <qcolor.h>
#include <qimage.h>
#include <qpainter.h>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include "texture.hpp"
#include "textureDTO.hpp"
#include "textureInfo.hpp"
#include <boost/algorithm/string.hpp>

using thewarrior::models::Texture;
using thewarrior::models::TextureInfo;

namespace commoneditor::ui {

EditTextureFormController::EditTextureFormController(std::unique_ptr<TextureDTO> originalTexture,
                                                     const std::vector<std::string> &allTextureNames,
                                                     const std::string &resourcesPath)
    : m_resourcesPath(resourcesPath),
      m_lastError(""),
      m_originalTexture(std::move(originalTexture)),
      m_allTextureNames(allTextureNames),
      m_gridColor(QColor::fromRgb(0, 0, 0)) {
          if (m_originalTexture != nullptr) {
              m_currentTexture = *m_originalTexture;
          } else {
              m_currentTexture.name = "";
              m_currentTexture.filename = "";
              m_currentTexture.width = 0;
              m_currentTexture.height = 0;
              m_currentTexture.tileWidth = 0;
              m_currentTexture.tileHeight = 0;
          }
}

const TextureDTO &EditTextureFormController::getOriginalTexture() const {
    return *m_originalTexture.get();
}

const std::string &EditTextureFormController::getResourcesPath() const {
    return m_resourcesPath;
}

const std::string &EditTextureFormController::getLastError() const {
    return m_lastError;
}

const std::shared_ptr<QPixmap> EditTextureFormController::getTextureToDisplay() const {
    return m_loadedTextureZoomed;
}

MonsterEditorTextureLoadResult EditTextureFormController::loadTextureImage(const std::string &filename) {
    QImage image;
    std::string imageFullPath { fmt::format("{0}/textures/{1}", m_resourcesPath, filename) };
    if (!image.load(imageFullPath.c_str())) {
        m_lastError = fmt::format("Unable to load the image {0}", imageFullPath);
        m_currentTexture.filename = "";
        m_currentTexture.width = 0;
        m_currentTexture.height = 0;
        return { false, 0, 0 };
    }
    m_loadedTexture = std::make_shared<QPixmap>(QPixmap::fromImage(image));
    m_currentTexture.filename = filename;
    m_currentTexture.width = m_loadedTexture->width();
    m_currentTexture.height = m_loadedTexture->height();
    drawTilesOnImage();
    setZoom(m_zoomPercentage);
    return { true, image.width(), image.height() };
}

void EditTextureFormController::drawTilesOnImage() {
    if (m_loadedTexture != nullptr) {
        m_loadedTextureWithTiles = std::make_shared<QPixmap>(*m_loadedTexture);
        QPainter painter(m_loadedTextureWithTiles.get());
        painter.setPen(QPen(m_gridColor));
        int tileWidth = m_currentTexture.tileWidth;
        int tileHeight = m_currentTexture.tileHeight;
        if (tileWidth > 0 && tileHeight > 0) {
            for (int y = 0; y < m_loadedTextureWithTiles->height(); y += tileHeight) {
                for (int x = 0; x < m_loadedTextureWithTiles->width(); x += tileWidth) {
                    painter.drawRect(x, y, tileWidth - 1, tileHeight - 1);
                }
            }
        }
        setZoom(m_zoomPercentage);
    } else {
        m_loadedTextureWithTiles = nullptr;
    }
}

void EditTextureFormController::setName(const std::string &name) {
    m_currentTexture.name = name;
}

void EditTextureFormController::setTileWidth(int value) {
    m_currentTexture.tileWidth = value;
    drawTilesOnImage();
}

void EditTextureFormController::setTileHeight(int value) {
    m_currentTexture.tileHeight = value;
    drawTilesOnImage();
}

void EditTextureFormController::setZoom(int percentage) {
    m_zoomPercentage = percentage;
    if (m_loadedTextureWithTiles != nullptr) {
        m_loadedTextureZoomed = std::make_shared<QPixmap>(*m_loadedTextureWithTiles);
        *m_loadedTextureZoomed = m_loadedTextureWithTiles->scaled(m_loadedTextureWithTiles->width() * m_zoomPercentage / 100,
                                                   m_loadedTextureWithTiles->height() * m_zoomPercentage / 100,
                                                   Qt::KeepAspectRatio,
                                                   Qt::FastTransformation);
    }
}

void EditTextureFormController::setGridColor(const QColor &color) {
    m_gridColor = color;
    drawTilesOnImage();
}

bool EditTextureFormController::validateTextureOperation(std::unique_ptr<TextureDTO> textureInfo) {
    if (textureInfo == nullptr) {
        m_lastError = "textureInfo must be provided.";
        return false;
    }
    TextureInfo textureCreationInfo {
        textureInfo->name,
        textureInfo->filename,
        textureInfo->width,
        textureInfo->height,
        textureInfo->tileWidth,
        textureInfo->tileHeight
    };
    try {
        auto texture = std::make_shared<Texture>(textureCreationInfo);
    }
    catch(std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    // Check if the texture name is available
    if (!isNameAvailable(textureInfo->name)) {
        m_lastError = fmt::format("The name {0} is already used.", textureInfo->name);
        return false;
    }
    return true;
}

bool EditTextureFormController::isNameAvailable(const std::string &name) const {
    auto sanitizedName = boost::to_lower_copy(boost::trim_copy(name));
    bool nameFound = std::find_if(m_allTextureNames.begin(),
            m_allTextureNames.end(),
            [sanitizedName](const std::string &texName) {
            return boost::to_lower_copy(boost::trim_copy(texName)) == sanitizedName;
            }) != m_allTextureNames.end();
    return (m_originalTexture == nullptr && !nameFound) ||
        (m_originalTexture != nullptr && (boost::to_lower_copy(boost::trim_copy(m_originalTexture->name)) == sanitizedName || !nameFound));
}

std::unique_ptr<TextureDTO> EditTextureFormController::createTextureDTOFromFields() const {
    return std::make_unique<TextureDTO>(m_currentTexture);
}

}  // namespace commoneditor::ui
