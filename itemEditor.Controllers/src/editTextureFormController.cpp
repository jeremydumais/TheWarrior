#include "editTextureFormController.hpp"
#include "texture.hpp"
#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <stdexcept>

namespace itemeditor::controllers {

EditTextureFormController::EditTextureFormController(std::unique_ptr<TextureDTO> originalTexture,
        const std::vector<std::string> &allTextureNames)
    : m_lastError(""),
    m_originalTexture(std::move(originalTexture)),
    m_allTextureNames(allTextureNames)
{
}

const TextureDTO &EditTextureFormController::getOriginalTexture() const
{
    return *m_originalTexture.get();
}

const std::string &EditTextureFormController::getLastError() const
{
    return m_lastError;
}

bool EditTextureFormController::validateTextureOperation(std::unique_ptr<TextureDTO> textureInfo)
{
    if (textureInfo == nullptr) {
        m_lastError = "textureInfo must be provided.";
        return false;
    }
    TextureInfo textureCreationInfo {
        textureInfo->name,
            textureInfo->filename,
            textureInfo->width,
            textureInfo->height,
            textureInfo->tileWidth,
            textureInfo->tileHeight
    };
    try {
        auto texture = std::make_shared<Texture>(textureCreationInfo);
    }
    catch(std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    //Check if the texture name is available
    if (!isNameAvailable(textureInfo->name)) {
        m_lastError = fmt::format("The name {0} is already used.", textureInfo->name);
        return false;
    }
    return true;
}

bool EditTextureFormController::isNameAvailable(const std::string &name) const
{
    auto sanitizedName = boost::to_lower_copy(boost::trim_copy(name));
    bool nameFound = std::find_if(m_allTextureNames.begin(),
            m_allTextureNames.end(),
            [sanitizedName](const std::string &texName) {
            return boost::to_lower_copy(boost::trim_copy(texName)) == sanitizedName;
            }) != m_allTextureNames.end();
    return (m_originalTexture == nullptr && !nameFound) ||
        (m_originalTexture != nullptr && (m_originalTexture->name == sanitizedName || !nameFound));

}

} // namespace itemeditor::controllers
