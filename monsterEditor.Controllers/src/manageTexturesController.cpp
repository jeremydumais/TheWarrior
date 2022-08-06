#include "manageTexturesController.hpp"
#include <algorithm>

namespace monstereditor::controllers {

ManageTextureController::ManageTextureController(TextureContainer &textureContainer)
    : m_lastError(""),
      m_textureContainer(textureContainer)
{
}

const std::string &ManageTextureController::getLastError() const
{
    return m_lastError;
}

std::vector<std::string> ManageTextureController::getTexturesNames() const
{
    std::vector<std::string> textureNames;
    const auto &textures { m_textureContainer.getTextures() };
    transform(textures.begin(),
              textures.end(),
              std::back_inserter(textureNames),
              [](const Texture & x) { return x.getName(); });
    return textureNames;
}

std::unique_ptr<TextureDTO> ManageTextureController::getTextureByName(const std::string &name) const
{
    auto texture = m_textureContainer.getTextureByName(name);
    if (texture.has_value()) {
        return std::unique_ptr<TextureDTO>(new TextureDTO {
            texture->get().getName(),
            texture->get().getFilename(),
            texture->get().getWidth(),
            texture->get().getHeight(),
            texture->get().getTileWidth(),
            texture->get().getTileHeight()
        } );
    }
    else {
        return nullptr;
    }
}

bool ManageTextureController::addTexture(std::unique_ptr<TextureDTO> info)
{
    auto textureCreationInfo = createTextureInfoFromDTO(std::move(info));
    if (!textureCreationInfo.has_value()) {
        return false;
    }
    return m_textureContainer.addTexture(textureCreationInfo.value());
}

bool ManageTextureController::replaceTexture(const std::string &name, std::unique_ptr<TextureDTO> updatedTexture)
{
    auto textureUpdatedInfo = createTextureInfoFromDTO(std::move(updatedTexture));
    if (!textureUpdatedInfo.has_value()) {
        return false;
    }
    return m_textureContainer.replaceTexture(name, textureUpdatedInfo.value());
}

bool ManageTextureController::removeTexture(const std::string &name)
{
    return m_textureContainer.removeTexture(name);
}

std::optional<TextureInfo> ManageTextureController::createTextureInfoFromDTO(std::unique_ptr<TextureDTO> textureDTO)
{
    if (textureDTO == nullptr) {
        m_lastError = "textureDTO must be provided.";
        return std::nullopt;
    }
    return TextureInfo {
        textureDTO->name,
        textureDTO->filename,
        textureDTO->width,
        textureDTO->height,
        textureDTO->tileWidth,
        textureDTO->tileHeight
    };
}

} // namespace monstereditor::controllers
