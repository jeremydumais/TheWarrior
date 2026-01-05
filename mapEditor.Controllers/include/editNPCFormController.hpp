#pragma once

#include <qpixmap.h>
#include <memory>
#include <string>
#include <vector>
#include "iTexturePixmapProvider.hpp"
#include "texture.hpp"

namespace mapeditor::controllers {

class EditNPCFormController {
 public:
    struct NPCPixmapResult {
        bool success;
        std::shared_ptr<QPixmap> result;
    };

    EditNPCFormController(const std::string &resourcesPath,
                          const std::vector<thewarrior::models::Texture> &textures,
                          commoneditor::ui::ITexturePixmapProvider &pixmapProvider);
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;
    const std::vector<thewarrior::models::Texture> &getTextures();
    NPCPixmapResult getNPCPixmap(const std::string &textureName,
                                 const int baseTextureIndex);

 private:
    std::string m_resourcesPath;
    std::string m_lastError = "";
    const std::vector<thewarrior::models::Texture> &m_textures;
    commoneditor::ui::ITexturePixmapProvider &m_texturePixmapProvider;
};

}  // namespace mapeditor::controllers
