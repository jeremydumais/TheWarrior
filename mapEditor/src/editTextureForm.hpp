
#ifndef EDITTEXTUREFORM_H
#define EDITTEXTUREFORM_H

#include "texture.hpp"
#include "ui_editTextureForm.h"
#include <string>
#include <vector>

class EditTextureForm : public QDialog
{
Q_OBJECT

public:
    explicit EditTextureForm(QWidget *parent,
            const std::string &resourcesPath,
            const Texture * const texture,
            const std::vector<std::string> &alreadyUsedNames);
    const TextureInfo &getTextureInfo() const;
private:
    Ui::editTextureFormClass ui;
    std::string m_resourcesPath;
    TextureInfo m_textureInfo;
    const std::vector<std::string> &m_alreadyUsedNames;
    bool m_isEditMode;
    void showErrorMessage(const std::string &message) const;
    void onPushButtonOK();
    void onPushButtonOpenFilenameClick();
};

#endif // EDITTEXTUREFORM_H
