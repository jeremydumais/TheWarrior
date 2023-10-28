#ifndef MAPEDITOR_SRC_MAINFORM_HPP_
#define MAPEDITOR_SRC_MAINFORM_HPP_

#include <qcombobox.h>
#include <qevent.h>
#include <qlabel.h>
#include <memory>
#include <qslider.h>
#include <string>
#include <vector>
#include "gameMap.hpp"
#include "mainController.hpp"
#include "components/mainForm_DebugInfoComponent.hpp"
#include "components/mainForm_GLComponent.hpp"
#include "components/mainForm_MapTabComponent.hpp"
#include "components/mainForm_MonsterZoneTabComponent.hpp"
#include "components/mainForm_TextureListTabComponent.hpp"
#include "components/mainForm_TextureSelectionComponent.hpp"
#include "components/mainForm_TileTabComponent.hpp"
#include "monsterZoneDTO.hpp"
#include "point.hpp"
#include "selectionMode.hpp"
#include "ui_mainForm.h"

class MainForm : public QMainWindow {
Q_OBJECT

 public:
    MainForm(QWidget *parent = nullptr,
            const std::string &currentFilePath = "");
    ~MainForm() override;
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
    MainForm_DebugInfoComponent m_debugInfoComponent;
    mapeditor::controllers::MainController m_controller;
    std::shared_ptr<QComboBox> comboBoxToolbarMonsterZone = nullptr;
    std::shared_ptr<QLabel> labelToolbarMonsterZoneColor = nullptr;
    std::shared_ptr<QLabel> labelToolbarZoom = nullptr;
    std::shared_ptr<QSlider> sliderZoom = nullptr;
    std::shared_ptr<QLabel> labelToolbarZoomValue = nullptr;
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
    void toggleViewMapConfiguration();
    void toggleViewTextureSelection();
    void toggleViewDebuggingInfo();
    void action_LightTheme_Click();
    void action_DarkTheme_Click();
    void action_DisplayGrid_Click();
    void action_ManageItemStore_Click();
    void action_ManageMonsterStore_Click();
    void setActiveToolbarActionChecked(SelectionMode mode);
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
    void onComboBoxToolbarMonsterZoneCurrentIndexChanged();
    void action_ApplyMonsterZone();
    void action_ClearMonsterZone();
    void sliderZoomValueChanged(int value);
    void openMap(const std::string &filePath);
    void saveMap(const std::string &filePath);
    void refreshWindowTitle();
    void refreshRecentMapsMenu();
    void addNewRecentMap(const std::string &filePath);
    void mapPaint(QPaintEvent *e);
    void resizeEvent(QResizeEvent *) override;
    void widgetMapConfigVisibilityChanged(bool visible);
    void widgetTextureSelectionVisibilityChanged(bool visible);
    void widgetDebugInfoVisibilityChanged(bool visible);
    void onTileSelected(thewarrior::models::MapTile *tile, thewarrior::models::Point<> coord);
    void onTextureAdded(thewarrior::models::TextureInfo textureInfo);
    void onTextureUpdated(const std::string &name, thewarrior::models::TextureInfo textureInfo);
    void onTextureDeleted(const std::string &name);
    void refreshTextureList();
    void onMonsterZoneAdded(mapeditor::controllers::MonsterZoneDTO monsterZoneDTO);
    void onMonsterZoneUpdated(const std::string &name, mapeditor::controllers::MonsterZoneDTO monsterZoneDTO);
    void onMonsterZoneDeleted(const std::string &name);
    void refreshMonsterZones();
    void toggleMonsterZoneAssignationControls();
    void useOnlyOneMonsterZoneChanged(bool value);
};

#endif  // MAPEDITOR_SRC_MAINFORM_HPP_

