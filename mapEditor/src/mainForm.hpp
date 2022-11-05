#ifndef MAINFORM_H
#define MAINFORM_H

#include "gameMap.hpp"
#include "mainController.hpp"
#include "components/mainForm_GLComponent.hpp"
#include "components/mainForm_MapTabComponent.hpp"
#include "components/mainForm_MonsterZoneTabComponent.hpp"
#include "components/mainForm_TextureListTabComponent.hpp"
#include "components/mainForm_TextureSelectionComponent.hpp"
#include "components/mainForm_TileTabComponent.hpp"
#include "point.hpp"
#include "selectionMode.hpp"
#include <memory>
#include <qcombobox.h>
#include <vector>
#include "ui_mainForm.h"

class MainForm : public QMainWindow
{
Q_OBJECT

public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();
    bool event(QEvent *event) override;
    void functionAfterShown();
    void setAppStylesheet(const std::string &style);
private:
    Ui::MainForm ui;
    MainForm_GLComponent m_glComponent;
    MainForm_MapTabComponent m_mapTabComponent;
    MainForm_TileTabComponent m_tileTabComponent;
    MainForm_MonsterZoneTabComponent m_monsterZoneTabComponent;
    MainForm_TextureListTabComponent m_textureListTabComponent;
    MainForm_TextureSelectionComponent m_textureSelectionComponent;
    mapeditor::controllers::MainController m_controller;
    std::shared_ptr<QComboBox> comboBoxToolbarMonsterZone = nullptr;
    std::string m_currentFilePath = "";
    bool m_functionAfterShownCalled = false;
    static const std::string THEME_PATH;
    static const std::string RECENT_MAPS;
    void connectUIActions();
    void action_Open_Click();
    void action_OpenRecentMap_Click();
    void action_Save_Click();
    void action_SaveAs_Click();
    void action_About_Click();
    void action_LightTheme_Click();
    void action_DarkTheme_Click();
    void action_DisplayGrid_Click();
    void action_ManageItemStore_Click();
    void action_ManageMonsterStore_Click();
    void action_SelectClick();
    void action_MoveMapClick();
    void action_ApplyTextureClick();
    void action_ApplyObjectClick();
    void action_EnableCanStepClick();
    void action_DisableCanStepClick();
    void action_ViewBorderModeClick();
    void action_BlockLeftBorderClick();
    void action_BlockTopBorderClick();
    void action_BlockRightBorderClick();
    void action_BlockBottomBorderClick();
    void action_ClearBlockedBordersClick();
    void openMap(const std::string &filePath);
    void saveMap(const std::string &filePath);
    void refreshWindowTitle();
    void refreshRecentMapsMenu();
    void addNewRecentMap(const std::string &filePath);
    void mapPaint(QPaintEvent *e);
    void resizeEvent(QResizeEvent *) override;
    void onTileSelected(thewarrior::models::MapTile *tile, thewarrior::models::Point<> coord);
    void onTextureAdded(thewarrior::models::TextureInfo textureInfo);
    void onTextureUpdated(const std::string &name, thewarrior::models::TextureInfo textureInfo);
    void onTextureDeleted(const std::string &name);
    void refreshTextureList();
    void refreshMonsterZones();
};

#endif // MAINFORM_H

