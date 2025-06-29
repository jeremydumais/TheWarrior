#ifndef MAPEDITOR_SRC_MAINFORM_HPP_
#define MAPEDITOR_SRC_MAINFORM_HPP_

#include <qcombobox.h>
#include <qevent.h>
#include <qlabel.h>
#include <qslider.h>
#include <memory>
#include <string>
#include <vector>
#include "mainController.hpp"
#include "components/debugInfoDockWidget.hpp"
#include "components/mainForm_GLComponent.hpp"
#include "components/mapPropsComponent.hpp"
#include "components/monsterZoneListComponent.hpp"
#include "components/textureListComponent.hpp"
#include "components/textureSelectionDockWidget.hpp"
#include "components/tilePropsComponent.hpp"
#include "mapTileDTO.hpp"
#include "monsterZoneDTO.hpp"
#include "qClosableDockWidget.hpp"
#include "selectionMode.hpp"
#include "ui_mainForm.h"

class MainForm : public QMainWindow {
Q_OBJECT

 public:
    explicit MainForm(QWidget *parent = nullptr,
            const std::string &currentFilePath = "");
    ~MainForm() override;
    bool event(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void functionAfterShown();

 private:
    Ui::MainForm ui;
    MainForm_GLComponent m_glComponent;
    std::shared_ptr<MapPropsComponent> m_mapPropsComponent = nullptr;
    std::shared_ptr<TilePropsComponent> m_tilePropsComponent = nullptr;
    std::shared_ptr<MonsterZoneListComponent> m_monsterZoneListComponent = nullptr;
    std::shared_ptr<TextureListComponent> m_textureListComponent = nullptr;
    std::shared_ptr<TextureSelectionDockWidget> m_textureSelectionDockWidget = nullptr;
    std::shared_ptr<DebugInfoDockWidget> m_debugInfoDockWidget = nullptr;
    mapeditor::controllers::MainController m_controller;
    std::shared_ptr<QComboBox> comboBoxToolbarMonsterZone = nullptr;
    std::shared_ptr<QLabel> labelToolbarMonsterZoneColor = nullptr;
    std::shared_ptr<QLabel> labelToolbarZoom = nullptr;
    std::shared_ptr<QSlider> sliderZoom = nullptr;
    std::shared_ptr<QLabel> labelToolbarZoomValue = nullptr;
    std::string m_currentFilePath = "";
    bool m_functionAfterShownCalled = false;
    bool m_closeFormRequested = false;
    void componentInitialization();
    void connectUIActions();
    void action_Open_Click();
    void action_OpenRecentMap_Click();
    void action_Save_Click();
    void action_SaveAs_Click();
    void action_About_Click();
    void toggleViewMapConfiguration();
    void changeViewMapConfigurationVisibility(bool visible);
    void toggleViewTextureSelection();
    void changeViewTextureSelectionVisibility(bool visible);
    void toggleViewDebuggingInfo();
    void changeViewDebuggingInfoVisibility(bool visible);
    void action_LightTheme_Click();
    void action_DarkTheme_Click();
    void action_DisplayGrid_Click();
    void action_ManageItemStore_Click();
    void action_ManageMonsterStore_Click();
    void setActiveToolbarActionChecked(SelectionMode mode);
    void action_SelectClick();
    void action_MoveMapClick();
    void action_PickerToolClick();
    void action_UndoClick();
    void action_RedoClick();
    void action_CopyClick();
    void action_PasteClick();
    void action_ApplyTextureClick();
    void action_ApplyObjectClick();
    void action_EnableCanStepClick();
    void action_DisableCanStepClick();
    void action_BlockLeftBorderClick();
    void action_BlockTopBorderClick();
    void action_BlockRightBorderClick();
    void action_BlockBottomBorderClick();
    void action_ClearBlockedBordersClick();
    void onComboBoxToolbarMonsterZoneCurrentIndexChanged();
    void action_ApplyMonsterZone();
    void action_ClearMonsterZone();
    void sliderZoomValueChanged(int value);
    void tabWidgetMapViewChanged(int index);
    void openMap(const std::string &filePath);
    void restorePersistedMenuState();
    void setAppStylesheet(const std::string &style);
    void refreshWindowTitle();
    void refreshUndoControls();
    void refreshClipboardControls();
    void refreshRecentMapsMenu();
    void addNewRecentMap(const std::string &filePath);
    void mapPaint(QPaintEvent *e);
    void resizeEvent(QResizeEvent *) override;
    void widgetMapConfigClosed(QEvent *event);
    void widgetTextureSelectionClosed(QEvent *event);
    void widgetDebugInfoClosed(QEvent *event);
    void onTileSelected(std::vector<mapeditor::controllers::MapTileDTO>);
    void onEditHistoryChanged();
    void onClipboardChanged();
    void onZoomChanged(int zoomPercentage);
    void onTextureAdded(commoneditor::ui::TextureDTO textureDTO);
    void onTextureUpdated(const std::string &name, commoneditor::ui::TextureDTO textureDTO);
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

