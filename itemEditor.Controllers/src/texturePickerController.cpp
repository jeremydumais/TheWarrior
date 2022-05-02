#include "texturePickerController.hpp"

TexturePickerController::TexturePickerController(const TextureContainer &textureContainer)
    : m_textureContainer(textureContainer)
{
}

const std::vector<Texture> &TexturePickerController::getTextures() const
{
    return m_textureContainer.getTextures();
}

std::optional<std::reference_wrapper<const Texture>> TexturePickerController::getTextureByName(const std::string &name) const
{
    return m_textureContainer.getTextureByName(name);
}
