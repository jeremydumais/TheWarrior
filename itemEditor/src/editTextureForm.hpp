
#ifndef ITEMEDITOR_SRC_EDITTEXTUREFORM_HPP_
#define ITEMEDITOR_SRC_EDITTEXTUREFORM_HPP_

#include <memory>
#include <string>
#include <vector>
#include "editTextureFormController.hpp"
#include "ui_editTextureForm.h"

class EditTextureForm : public QDialog {
Q_OBJECT

 public:
    explicit EditTextureForm(QWidget *parent,
            const std::string &resourcesPath,
            std::unique_ptr<commoneditor::ui::TextureDTO> originalTexture,
            const std::vector<std::string> &allTextureNames);
    std::unique_ptr<commoneditor::ui::TextureDTO> getTextureInfo() const;

 private:
    Ui::editTextureFormClass ui;
    bool m_isEditMode;
    commoneditor::ui::EditTextureFormController m_controller;
    void loadExistingItemToForm();
    void refreshTextureImage();
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

#endif  // ITEMEDITOR_SRC_EDITTEXTUREFORM_HPP_
