#ifndef TEXTUREPICKERFORM_H
#define TEXTUREPICKERFORM_H

#include "ui_texturePickerForm.h"
#include "textureContainer.hpp"
#include "texturePickerController.hpp"
#include <qpixmap.h>
#include <memory.h>
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
            const TextureContainer &textureContainer);
    void connectUIActions();
    void setCurrentSelection(const std::string &textureName,
            int textureIndex);
    PickerResult getResult() const;
private:
    Ui::texturePickerFormClass ui;
    const std::string &m_resourcesPath;
    TexturePickerController m_controller;
    std::shared_ptr<QPixmap> m_loadedTexture = nullptr;
    void onPushButtonCancelClick();
    void onPushButtonOKClick();
    void refreshTextureComboBox();
    void onLabelImageTextureMouseReleaseEvent(QMouseEvent *event);
    void displaySelectedTile(const std::string &textureName,
            int textureIndex);
    void refreshZoomDisplayValue();
    void onComboBoxTextureCurrentIndexChanged();
    void onHorizontalSliderZoomChanged(int value);
};

#endif // TEXTUREPICKERFORM_H
