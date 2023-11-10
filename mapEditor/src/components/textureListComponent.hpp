#pragma once

#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <memory>
#include <optional>
#include <string>
#include "mainForm_GLComponent.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include "textureDTO.hpp"
#include "ui_textureListComponent.h"

class TextureListComponent : public QWidget {
Q_OBJECT
 public:
    TextureListComponent(QWidget *parent, MainForm_GLComponent *glComponent);
    void connectUIActions();
    void refreshTextureList();
    std::optional<std::reference_wrapper<const thewarrior::models::Texture>> getSelectedTextureInTextureList() const;

 signals:
    void textureAdded(commoneditor::ui::TextureDTO textureDTO);
    void textureUpdated(const std::string &name, commoneditor::ui::TextureDTO textureDTO);
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
