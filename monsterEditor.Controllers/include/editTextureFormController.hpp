#pragma once

#include <qpixmap.h>
#include <memory>
#include <string>
#include <vector>
#include "textureDTO.hpp"

namespace monstereditor::controllers {

struct MonsterEditorTextureLoadResult {
    bool Success;
    int TextureWidth;
    int TextureHeight;
};

class EditTextureFormController {
 public:
    EditTextureFormController(std::unique_ptr<TextureDTO> originalTexture,
                              const std::vector<std::string> &allTextureNames,
            const std::string &resourcesPath);
    const TextureDTO &getOriginalTexture() const;
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;
    const std::shared_ptr<QPixmap> getTextureToDisplay() const;
    MonsterEditorTextureLoadResult loadTextureImage(const std::string &filename);
    void drawTilesOnImage();
    void setName(const std::string &name);
    void setTileWidth(int value);
    void setTileHeight(int value);
    void setZoom(int percentage);
    void setGridColor(const QColor &color);
    bool validateTextureOperation(std::unique_ptr<TextureDTO> textureInfo);
    std::unique_ptr<TextureDTO> createTextureDTOFromFields() const;

 private:
    std::string m_resourcesPath;
    std::string m_lastError;
    std::unique_ptr<TextureDTO> m_originalTexture;
    TextureDTO m_currentTexture;
    const std::vector<std::string> &m_allTextureNames;
    std::shared_ptr<QPixmap> m_loadedTexture = nullptr;
    std::shared_ptr<QPixmap> m_loadedTextureWithTiles = nullptr;
    std::shared_ptr<QPixmap> m_loadedTextureZoomed = nullptr;
    int m_zoomPercentage = 100;
    QColor m_gridColor;
    bool isNameAvailable(const std::string &name) const;
};

}  // namespace monstereditor::controllers
