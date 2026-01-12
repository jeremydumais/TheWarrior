#pragma once

#include <qpixmap.h>
#include <memory>
#include <string>
#include <vector>
#include "iTexturePixmapProvider.hpp"
#include "npcDTO.hpp"
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
                          commoneditor::ui::ITexturePixmapProvider &pixmapProvider,
                          const std::optional<mapeditor::controllers::NPCDTO> selectedNPC,
                          const std::vector<std::string> &alreadyUsedNPCIds);
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;
    bool isEditMode() const;
    const std::vector<thewarrior::models::Texture> &getTextures();
    NPCPixmapResult getNPCPixmap(const std::string &textureName,
                                 const int baseTextureIndex);
    bool isNPCIdAlreadyUsed(const std::string &id) const;
    static std::vector<std::string> convertPlainTextToLines(const QString &text);
    bool isDTOValid(mapeditor::controllers::NPCDTO &dto);

 private:
    std::string m_resourcesPath;
    std::string m_lastError = "";
    const std::vector<thewarrior::models::Texture> &m_textures;
    commoneditor::ui::ITexturePixmapProvider &m_texturePixmapProvider;
    const std::optional<mapeditor::controllers::NPCDTO> m_selectedNPC;
    const std::vector<std::string> &m_alreadyUsedNPCIds;
};

}  // namespace mapeditor::controllers
