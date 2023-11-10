
#ifndef MAPEDITOR_SRC_EDITTEXTUREFORM_HPP_
#define MAPEDITOR_SRC_EDITTEXTUREFORM_HPP_

#include <memory>
#include <string>
#include <vector>
#include "texture.hpp"
#include "editTextureFormController.hpp"
#include "textureDTO.hpp"
#include "ui_editTextureForm.h"

class EditTextureForm : public QDialog {
Q_OBJECT

 public:
    explicit EditTextureForm(QWidget *parent,
            const std::string &resourcesPath,
            const thewarrior::models::Texture * const texture,
            const std::vector<std::string> &alreadyUsedNames);
    commoneditor::ui::TextureDTO getTextureInfo() const;

 private:
    Ui::editTextureFormClass ui;
    std::unique_ptr<commoneditor::ui::EditTextureFormController> m_controller;
    bool m_isEditMode;
    void loadExistingItemToForm();
    void refreshTextureImage();
    void showErrorMessage(const std::string &message) const;
    void refreshZoomDisplayValue();
    void onPushButtonOK();
    void onLineEditNameTextChanged(const QString &text);
    void onPushButtonOpenFilenameClick();
    void onSpinBoxTileWidthValueChanged(int value);
    void onSpinBoxTileHeightValueChanged(int value);
    void onHorizontalSliderZoomChanged(int value);
    void onPushButtonGridBlackClick();
    void onPushButtonGridWhiteClick();
};

#endif  // MAPEDITOR_SRC_EDITTEXTUREFORM_HPP_
