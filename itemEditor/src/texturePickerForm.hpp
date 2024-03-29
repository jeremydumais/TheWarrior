#ifndef TEXTUREPICKERFORM_H
#define TEXTUREPICKERFORM_H

#include "ui_texturePickerForm.h"
#include "textureContainer.hpp"
#include "texturePickerController.hpp"
#include <string>

struct PickerResult
{
    std::string textureName;
    int textureIndex;
};

class TexturePickerForm : public QDialog
{
Q_OBJECT

public:
    TexturePickerForm(QWidget *parent,
            const std::string &resourcesPath,
            const thewarrior::models::TextureContainer &textureContainer);
    void connectUIActions();
    void setCurrentSelection(const std::string &textureName,
            int textureIndex);
    PickerResult getResult() const;
private:
    Ui::texturePickerFormClass ui;
    const std::string &m_resourcesPath;
    itemeditor::controllers::TexturePickerController m_controller;
    void onPushButtonCancelClick();
    void onPushButtonOKClick();
    void refreshTextureComboBox();
    void onLabelImageTextureMouseReleaseEvent(QMouseEvent *event);
    void displaySelectedTile(const std::string &textureName,
            int textureIndex);
    void onComboBoxTextureCurrentIndexChanged();
};

#endif // TEXTUREPICKERFORM_H
