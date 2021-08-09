#pragma once

#include "mainForm_GLComponent.hpp"
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

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
    boost::optional<const Texture &> getSelectedTextureInTextureList();
signals:
    void textureAdded(TextureInfo textureInfo);
    void textureUpdated(const std::string &name, TextureInfo textureInfo);
    void textureDeleted(const std::string &name);
private:
    MainForm_GLComponent *glComponent;
    QListWidget *listWidgetTextures;
    QPushButton *pushButtonAddTexture;
    QPushButton *pushButtonEditTexture;
    QPushButton *pushButtonDeleteTexture;
    void onPushButtonAddTextureClick();
	void onPushButtonEditTextureClick();
	void onPushButtonDeleteTextureClick();
};