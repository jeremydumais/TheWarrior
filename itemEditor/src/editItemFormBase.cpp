#include "editItemFormBase.hpp"
#include "texturePickerForm.hpp"
#include <boost/algorithm/string.hpp>

EditItemFormBase::EditItemFormBase(QWidget *parent,
        const std::string &resourcesPath,
        std::optional<std::string> itemIdToEdit)
    : QDialog(parent),
    m_lastError(""),
    m_resourcesPath(resourcesPath),
    m_itemIdToEdit(itemIdToEdit)
{
}

std::optional<TextureSelectionInfo> EditItemFormBase::showTexturePicker(const TextureSelectionInfo &info,
        const TextureContainer &textureContainer)
{
    TexturePickerForm texturePickerForm(this,
            m_resourcesPath,
            textureContainer);
    auto selectedTexture = info.textureName;
    if (!boost::trim_copy(selectedTexture).empty()) {
        texturePickerForm.setCurrentSelection(selectedTexture,
                info.textureIndex);
    }
    if (texturePickerForm.exec() == QDialog::Accepted) {
        const auto &result = texturePickerForm.getResult();
        return TextureSelectionInfo {
            result.textureName,
                result.textureIndex
        };
    }
    return std::nullopt;
}
