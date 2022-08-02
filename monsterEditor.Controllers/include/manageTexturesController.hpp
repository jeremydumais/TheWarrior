#pragma once

#include "textureContainer.hpp"
#include "textureDTO.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class ManageTextureController
{
public:
    ManageTextureController(TextureContainer &textureContainer);
    const std::string &getLastError() const;
    std::vector<std::string> getTexturesNames() const;
    std::unique_ptr<TextureDTO> getTextureByName(const std::string &name) const;
    bool addTexture(std::unique_ptr<TextureDTO> info);
    bool replaceTexture(const std::string &name, std::unique_ptr<TextureDTO>  updatedTexture);
    bool removeTexture(const std::string &name);
private:
    std::string m_lastError;
    TextureContainer &m_textureContainer;
    std::optional<TextureInfo> createTextureInfoFromDTO(std::unique_ptr<TextureDTO> textureDTO);
};