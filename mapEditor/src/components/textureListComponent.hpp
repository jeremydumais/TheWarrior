#pragma once

#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <optional>
#include <string>
#include "mainForm_GLComponent.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include "ui_textureListComponent.h"

class TextureListComponent : public QWidget {
Q_OBJECT
 public:
    TextureListComponent(QWidget *parent, MainForm_GLComponent *glComponent);
    void connectUIActions();
    void refreshTextureList();
    std::optional<std::reference_wrapper<const thewarrior::models::Texture>> getSelectedTextureInTextureList() const;

 signals:
    void textureAdded(thewarrior::models::TextureInfo textureInfo);
    void textureUpdated(const std::string &name, thewarrior::models::TextureInfo textureInfo);
    void textureDeleted(const std::string &name);

 private:
    Ui::TextureListComponent ui;
    QTableWidgetKeyPressWatcher tableWidgetTextureKeyWatcher;
    MainForm_GLComponent *m_glComponent;
    void onPushButtonAddTextureClick();
    void onPushButtonEditTextureClick();
    void onPushButtonDeleteTextureClick();
    void onTableWidgetTexturesKeyPressEvent(int key, int, int);
};
