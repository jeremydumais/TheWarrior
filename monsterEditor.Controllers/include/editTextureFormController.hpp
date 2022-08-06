#pragma once

#include "textureDTO.hpp"
#include <memory>
#include <string>
#include <vector>

namespace MonsterEditorControllers
{
class EditTextureFormController
{
public:
    EditTextureFormController(std::unique_ptr<TextureDTO> originalTexture,
                              const std::vector<std::string> &allTextureNames);
    const TextureDTO &getOriginalTexture() const;
    const std::string &getLastError() const;
    bool validateTextureOperation(std::unique_ptr<TextureDTO> textureInfo);
private:
    std::string m_lastError;
    std::unique_ptr<TextureDTO> m_originalTexture;
    const std::vector<std::string> &m_allTextureNames;
    bool isNameAvailable(const std::string &name) const;
};
} // namespace MonsterEditorControllers
