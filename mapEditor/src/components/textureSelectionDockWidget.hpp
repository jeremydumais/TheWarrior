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
    void setDisplayGrid(bool enabled);

 signals:
    void displayGridChanged(bool enabled);

 private:
    Ui::TextureSelectionDockWidget ui;
    MainForm_GLComponent *m_glComponent;
    QPixmap m_texturePixmap;
    QRect m_atlasSelection;
    QPoint m_atlasPressPosition;
    QPoint m_atlasSelectionStart;
    bool m_selectingAtlas = false;
    bool m_draggingAtlasSelection = false;
    bool eventFilter(QObject *watched, QEvent *event) override;
    bool isMultiAtlasMode() const;
    QPoint atlasTileAt(QPoint position) const;
    void updateTexturePreview();
    void startAtlasDrag();
    void onPushButtonSelectedTextureClearClick();
    void onPushButtonSelectedObjectClearClick();
    void onLabelImageTextureMouseReleaseEvent(QMouseEvent *event);
    void onComboBoxTextureCurrentIndexChanged();
    void onPickerToolTileSelected(const PickerToolSelection &selection);
};
