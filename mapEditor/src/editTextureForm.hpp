
#ifndef MAPEDITOR_SRC_EDITTEXTUREFORM_HPP_
#define MAPEDITOR_SRC_EDITTEXTUREFORM_HPP_

#include <string>
#include <vector>
#include "texture.hpp"
#include "ui_editTextureForm.h"

class EditTextureForm : public QDialog {
Q_OBJECT

 public:
    explicit EditTextureForm(QWidget *parent,
            const std::string &resourcesPath,
            const thewarrior::models::Texture * const texture,
            const std::vector<std::string> &alreadyUsedNames);
    const thewarrior::models::TextureInfo &getTextureInfo() const;

 private:
    Ui::editTextureFormClass ui;
    std::string m_resourcesPath;
    thewarrior::models::TextureInfo m_textureInfo;
    const std::vector<std::string> &m_alreadyUsedNames;
    bool m_isEditMode;
    void showErrorMessage(const std::string &message) const;
    void onPushButtonOK();
    void onPushButtonOpenFilenameClick();
};

#endif  // MAPEDITOR_SRC_EDITTEXTUREFORM_HPP_
