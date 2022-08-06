#ifndef EDITTEXTUREFORM_H
#define EDITTEXTUREFORM_H

#include "editTextureFormController.hpp"
#include "textureDTO.hpp"
#include "ui_editTextureForm.h"
#include <qpixmap.h>
#include <memory>
#include <string>
#include <vector>


struct MonsterEditorTextureLoadResult
{
    bool Success;
    int TextureWidth;
    int TextureHeight;
};

class EditTextureForm : public QDialog
{
Q_OBJECT

public:
    explicit EditTextureForm(QWidget *parent,
            const std::string &resourcesPath,
            std::unique_ptr<MonsterEditorControllers::TextureDTO> originalTexture,
            const std::vector<std::string> &allTextureNames);
    std::unique_ptr<MonsterEditorControllers::TextureDTO> getTextureInfo() const;
private:
    Ui::editTextureFormClass ui;
    bool m_isEditMode;
    MonsterEditorControllers::EditTextureFormController m_controller;
    std::string m_resourcesPath;
    std::shared_ptr<QPixmap> m_loadedTexture = nullptr;
    void loadExistingItemToForm();
    MonsterEditorTextureLoadResult loadTextureImage(const std::string &filename);
    void refreshZoomDisplayValue();
    void onPushButtonOK();
    void onPushButtonOpenFilenameClick();
    void onHorizontalSliderZoomChanged(int value);
    std::unique_ptr<MonsterEditorControllers::TextureDTO> createTextureDTOFromFields() const;
};

#endif // EDITTEXTUREFORM_H
