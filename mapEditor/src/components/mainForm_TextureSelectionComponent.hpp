#pragma once

#include "mainForm_GLComponent.hpp"
#include "point.hpp"
#include "qClickableLabel.hpp"
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
    MainForm_GLComponent *m_glComponent;
    QComboBox *m_comboBoxTexture;
    QLabel *m_labelSelectedTexture;
    QPushButton *m_pushButtonSelectedTextureClear;
    QLabel *m_labelSelectedObject;
    QPushButton *m_pushButtonSelectedObjectClear;
    QClickableLabel *m_labelImageTexture;
	//QPixmap getTextureTileImageFromTexture(int tileIndex, const Texture &texture) const;
    void onPushButtonSelectedTextureClearClick();
	void onPushButtonSelectedObjectClearClick();
	void onLabelImageTextureMouseReleaseEvent(QMouseEvent *event);
    void onComboBoxTextureCurrentIndexChanged();
};