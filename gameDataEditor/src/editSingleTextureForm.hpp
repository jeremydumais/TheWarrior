#ifndef GAMEDATAEDITOR_SRC_EDITSINGLETEXTUREFORM_HPP_
#define GAMEDATAEDITOR_SRC_EDITSINGLETEXTUREFORM_HPP_

#include <memory>
#include <string>
#include <vector>
#include "editTextureFormController.hpp"
#include "ui_editSingleTextureForm.h"

class EditSingleTextureForm : public QDialog {
Q_OBJECT

 public:
    explicit EditSingleTextureForm(QWidget *parent,
            const std::string &resourcesPath,
            std::unique_ptr<commoneditor::ui::TextureDTO> originalTexture);
    std::unique_ptr<commoneditor::ui::TextureDTO> getTextureInfo() const;

 private:
    Ui::editSingleTextureFormClass ui;
    bool m_isEditMode;
    std::vector<std::string> m_emptyNameVector;
    commoneditor::ui::EditTextureFormController m_controller;
    void loadExistingItemToForm();
    void refreshTextureImage();
    void refreshZoomDisplayValue();
    void onPushButtonOK();
    void onPushButtonOpenFilenameClick();
    void onSpinBoxTileWidthValueChanged(int value);
    void onSpinBoxTileHeightValueChanged(int value);
    void onHorizontalSliderZoomChanged(int value);
    void onPushButtonGridBlackClick();
    void onPushButtonGridWhiteClick();
};

#endif  // GAMEDATAEDITOR_SRC_EDITSINGLETEXTUREFORM_HPP_
