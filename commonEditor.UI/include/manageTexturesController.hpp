#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "textureContainer.hpp"
#include "textureDTO.hpp"

namespace commoneditor::ui {

class ManageTextureController {
 public:
    explicit ManageTextureController(thewarrior::models::TextureContainer &textureContainer);
    const std::string &getLastError() const;
    std::vector<std::string> getTexturesNames() const;
    std::unique_ptr<commoneditor::ui::TextureDTO> getTextureByName(const std::string &name) const;
    bool addTexture(std::unique_ptr<commoneditor::ui::TextureDTO> info);
    bool replaceTexture(const std::string &name, std::unique_ptr<commoneditor::ui::TextureDTO>  updatedTexture);
    bool removeTexture(const std::string &name);

 private:
    std::string m_lastError;
    thewarrior::models::TextureContainer &m_textureContainer;
    std::optional<thewarrior::models::TextureInfo> createTextureInfoFromDTO(std::unique_ptr<commoneditor::ui::TextureDTO> textureDTO);
};

}  // namespace commoneditor::ui
