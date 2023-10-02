#ifndef MONSTEREDITOR_SRC_EDITTEXTUREFORM_HPP_
#define MONSTEREDITOR_SRC_EDITTEXTUREFORM_HPP_

#include <qpixmap.h>
#include <memory>
#include <string>
#include <vector>
#include "editTextureFormController.hpp"
#include "textureDTO.hpp"
#include "ui_editTextureForm.h"


struct MonsterEditorTextureLoadResult {
    bool Success;
    int TextureWidth;
    int TextureHeight;
};

class EditTextureForm : public QDialog {
Q_OBJECT

 public:
    explicit EditTextureForm(QWidget *parent,
            const std::string &resourcesPath,
            std::unique_ptr<monstereditor::controllers::TextureDTO> originalTexture,
            const std::vector<std::string> &allTextureNames);
    std::unique_ptr<monstereditor::controllers::TextureDTO> getTextureInfo() const;
 private:
    Ui::editTextureFormClass ui;
    bool m_isEditMode;
    monstereditor::controllers::EditTextureFormController m_controller;
    std::string m_resourcesPath;
    std::shared_ptr<QPixmap> m_loadedTexture = nullptr;
    std::shared_ptr<QPixmap> m_loadedTextureWithTiles = nullptr;
    void loadExistingItemToForm();
    MonsterEditorTextureLoadResult loadTextureImage(const std::string &filename);
    void drawTilesOnImage();
    void refreshZoomDisplayValue();
    void onPushButtonOK();
    void onPushButtonOpenFilenameClick();
    void onHorizontalSliderZoomChanged(int value);
    std::unique_ptr<monstereditor::controllers::TextureDTO> createTextureDTOFromFields() const;
};

#endif  // MONSTEREDITOR_SRC_EDITTEXTUREFORM_HPP_
