#pragma once

#include "mainForm_GLComponent.hpp"
#include "point.hpp"
#include "qClickableLabel.hpp"
#include "textureSelectionComponentController.hpp"
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

struct MainForm_TextureSelectionComponent_Objects
{
    MainForm_GLComponent *glComponent = nullptr;
    QComboBox *comboBoxTexture = nullptr;
    QLabel *labelSelectedTexture = nullptr;
    QPushButton *pushButtonSelectedTextureClear = nullptr;
    QLabel *labelSelectedObject = nullptr;
    QPushButton *pushButtonSelectedObjectClear = nullptr;
    QClickableLabel *labelImageTexture = nullptr;
};

class MainForm_TextureSelectionComponent : public QWidget
{
    Q_OBJECT
public:
    MainForm_TextureSelectionComponent();
    void initializeUIObjects(const MainForm_TextureSelectionComponent_Objects &objects);
    void connectUIActions();
    void refreshTextureList();
	void displaySelectedTextureImage();
signals:
private:
    TextureSelectionComponentController controller;
    MainForm_GLComponent *glComponent;
    QComboBox *comboBoxTexture;
    QLabel *labelSelectedTexture;
    QPushButton *pushButtonSelectedTextureClear;
    QLabel *labelSelectedObject;
    QPushButton *pushButtonSelectedObjectClear;
    QClickableLabel *labelImageTexture;
	QPixmap getTextureTileImageFromTexture(int tileIndex, const Texture &texture) const;
    void onPushButtonSelectedTextureClearClick();
	void onPushButtonSelectedObjectClearClick();
	void onLabelImageTextureMouseReleaseEvent(QMouseEvent *event);
    void onComboBoxTextureCurrentIndexChanged();
};