#pragma once

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <qdockwidget.h>
#include "mainForm_GLComponent.hpp"
#include "pickerToolSelection.hpp"
#include "qClickableLabel.hpp"
#include "qClosableDockWidget.hpp"
#include "ui_textureSelectionDockWidget.h"

class TextureSelectionDockWidget : public QClosableDockWidget {
Q_OBJECT
 public:
    TextureSelectionDockWidget(QWidget *parent, MainForm_GLComponent *glComponent);
    void connectUIActions();
    void refreshTextureList();
    void displaySelectedTextureImage();

 signals:

 private:
    Ui::TextureSelectionDockWidget ui;
    MainForm_GLComponent *m_glComponent;
    void onPushButtonSelectedTextureClearClick();
    void onPushButtonSelectedObjectClearClick();
    void onLabelImageTextureMouseReleaseEvent(QMouseEvent *event);
    void onComboBoxTextureCurrentIndexChanged();
    void onPickerToolTileSelected(const PickerToolSelection &selection);
};
