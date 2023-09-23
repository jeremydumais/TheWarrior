#ifndef EDITITEMFORMBASE_H
#define EDITITEMFORMBASE_H

#include "itemStore.hpp"
#include "textureContainer.hpp"
#include <QtWidgets/qdialog.h>
#include <memory>
#include <optional>
#include <string>

struct TextureSelectionInfo {
    std::string textureName;
    int textureIndex;
};

class EditItemFormBase : public QDialog
{
public:
    EditItemFormBase(QWidget *parent,
				     const std::string &resourcesPath,
                     std::optional<std::string> itemIdToEdit);
    std::optional<TextureSelectionInfo> showTexturePicker(const TextureSelectionInfo &info,
                                                          const thewarrior::models::TextureContainer &textureContainer);
protected:
    std::string m_lastError;
	const std::string &m_resourcesPath;
    std::optional<std::string> m_itemIdToEdit;
};

#endif // EDITITEMFORMBASE_H
