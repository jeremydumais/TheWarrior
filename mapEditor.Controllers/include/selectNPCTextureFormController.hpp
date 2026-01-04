#pragma once

#include <qpixmap.h>
#include <string>
#include <vector>
#include "iTexturePixmapProvider.hpp"
#include "texture.hpp"

namespace mapeditor::controllers {

class SelectNPCTextureFormController {
 public:
    struct AvailableNPC {
        int baseTextureIndex;
        QPixmap icon;
    };

    struct AvailableNPCResult {
        bool success;
        std::vector<AvailableNPC> result {};
    };

    struct NPCAnimation {
        bool success;
        std::vector<QPixmap> walkDown {};
        std::vector<QPixmap> walkLeft {};
        std::vector<QPixmap> walkRight {};
        std::vector<QPixmap> walkUp {};
    };

    SelectNPCTextureFormController(const std::string &resourcesPath,
                                   const std::vector<thewarrior::models::Texture> &textures,
                                   commoneditor::ui::ITexturePixmapProvider &pixmapProvider);
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;
    std::vector<std::string> getTextureNames() const;
    AvailableNPCResult getAvailableNPCs(const std::string &textureName);
    NPCAnimation getNPCAnimationTiles(const std::string &textureName, int baseNPCIndex);

 private:
    std::string m_resourcesPath;
    std::string m_lastError = "";
    const std::vector<thewarrior::models::Texture> &m_textures;
    commoneditor::ui::ITexturePixmapProvider &m_pixmapProvider;
    std::vector<QPixmap> getNPCMovementsByBaseIndex(const QPixmap *sourcePixmap,
                                                    int baseIndex,
                                                    const thewarrior::models::Texture &texture);
};

}  // namespace mapeditor::controllers
