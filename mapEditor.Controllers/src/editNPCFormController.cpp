#include <string>
#include <vector>
#include "editNPCFormController.hpp"
#include "texture.hpp"

using thewarrior::models::Texture;

namespace mapeditor::controllers {

EditNPCFormController::EditNPCFormController(const std::string &resourcesPath,
                                             const std::vector<Texture> &textures)
: m_resourcesPath(resourcesPath),
  m_textures(textures) {
}

const std::string &EditNPCFormController::getResourcesPath() const {
    return m_resourcesPath;
}

const std::string &EditNPCFormController::getLastError() const {
    return m_lastError;
}

const std::vector<thewarrior::models::Texture> &EditNPCFormController::getTextures() {
    return m_textures;
}

}  // namespace mapeditor::controllers
