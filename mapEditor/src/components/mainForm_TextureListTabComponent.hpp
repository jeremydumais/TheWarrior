#pragma once

#include "mainForm_GLComponent.hpp"
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <optional>

struct MainForm_TextureListTabComponent_Objects
{
    MainForm_GLComponent *glComponent = nullptr;
    QListWidget *listWidgetTextures = nullptr;
    QPushButton *pushButtonAddTexture = nullptr;
    QPushButton *pushButtonEditTexture = nullptr;
    QPushButton *pushButtonDeleteTexture = nullptr;
};

class MainForm_TextureListTabComponent : public QWidget
{
    Q_OBJECT
public:
    MainForm_TextureListTabComponent();
    void initializeUIObjects(const MainForm_TextureListTabComponent_Objects &objects);
    void connectUIActions();
    void refreshTextureList();
    std::optional<std::reference_wrapper<const thewarrior::models::Texture>> getSelectedTextureInTextureList() const;
signals:
    void textureAdded(thewarrior::models::TextureInfo textureInfo);
    void textureUpdated(const std::string &name, thewarrior::models::TextureInfo textureInfo);
    void textureDeleted(const std::string &name);
private:
    MainForm_GLComponent *m_glComponent;
    QListWidget *m_listWidgetTextures;
    QPushButton *m_pushButtonAddTexture;
    QPushButton *m_pushButtonEditTexture;
    QPushButton *m_pushButtonDeleteTexture;
    void onPushButtonAddTextureClick();
	void onPushButtonEditTextureClick();
	void onPushButtonDeleteTextureClick();
};
