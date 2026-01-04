#pragma once

#include <string>
#include <vector>
#include "texture.hpp"

namespace mapeditor::controllers {

class EditNPCFormController {
 public:
    EditNPCFormController(const std::string &resourcesPath,
                          const std::vector<thewarrior::models::Texture> &textures);
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;
    const std::vector<thewarrior::models::Texture> &getTextures();

 private:
    std::string m_resourcesPath;
    std::string m_lastError = "";
    const std::vector<thewarrior::models::Texture> &m_textures;
};

}  // namespace mapeditor::controllers
