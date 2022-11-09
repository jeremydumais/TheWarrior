#include "mainForm.hpp"
#include "aboutBoxForm.hpp"
#include "components/mainForm_MonsterZoneTabComponent.hpp"
#include "configurationManager.hpp"
#include "errorMessage.hpp"
#include "gameMapStorage.hpp"
#include "manageItemStoreForm.hpp"
#include "manageMonsterStoreForm.hpp"
#include "specialFolders.hpp"
#include <QtCore/qfile.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <fstream>
#include <qcombobox.h>
#include <qtimer.h>

using namespace std;
using namespace commoneditor::ui;
using namespace thewarrior::models;
using namespace thewarrior::storage;

const std::string MainForm::THEME_PATH { "Display.Theme" };
const std::string MainForm::RECENT_MAPS { "Map.Recents" };

MainForm::MainForm(QWidget *parent)
    : QMainWindow(parent),
    ui(Ui::MainForm())
{
    ui.setupUi(this);

    m_controller.initializeExecutablePath();
    m_controller.initializeResourcesPath();
    m_controller.initializeUserConfigFolder();

    //Check if the user configuration folder exist
    const auto &userConfigFolder = m_controller.getUserConfigFolder();
    if (!boost::filesystem::exists(userConfigFolder)) {
        if (!boost::filesystem::create_directory(userConfigFolder)) {
            ErrorMessage::show(fmt::format("Unable to create the folder {0}", userConfigFolder), "");
            exit(1);
        }
    }

    //Check if the configuration file exist
    ConfigurationManager configManager(userConfigFolder + "config.json");
    if (!configManager.fileExists()) {
        //Try to create a default configuration
        if (!configManager.save()) {
            ErrorMessage::show("An error occurred while creation a default the configuration file.",
                    configManager.getLastError());
        }
    }
    if (configManager.load()) {
        setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
    }
    else {
        ErrorMessage::show("An error occurred while loading the configuration file.",
                configManager.getLastError());
    }

    m_controller.loadConfiguredMonsterStores();

    m_glComponent.initializeUIObjects(ui.mapOpenGLWidget);
    m_glComponent.setResourcesPath(m_controller.getResourcesPath());
    m_glComponent.setSelectionMode(SelectionMode::Select);
    //MapTab Component initialization
    MainForm_MapTabComponent_Objects mapUIObjects;
    mapUIObjects.glComponent = &m_glComponent;
    mapUIObjects.lineEditMapWidth = ui.lineEditMapWidth;
    mapUIObjects.lineEditMapHeight = ui.lineEditMapHeight;
    mapUIObjects.spinBoxMapSizeTop = ui.spinBoxMapSizeTop;
    mapUIObjects.spinBoxMapSizeLeft = ui.spinBoxMapSizeLeft;
    mapUIObjects.spinBoxMapSizeRight = ui.spinBoxMapSizeRight;
    mapUIObjects.spinBoxMapSizeBottom = ui.spinBoxMapSizeBottom;
    mapUIObjects.pushButtonApplySizeChange = ui.pushButtonApplySizeChange;
    m_mapTabComponent.initializeUIObjects(mapUIObjects);
    //TileTab Component initialization
    MainForm_TileTabComponent_Objects tileUIObjects;
    tileUIObjects.glComponent = &m_glComponent;
    tileUIObjects.labelTileCoordXY = ui.labelTileCoordXY;
    tileUIObjects.lineEditTexName = ui.lineEditTexName;
    tileUIObjects.spinBoxTexIndex = ui.spinBoxTexIndex;
    tileUIObjects.lineEditObjTexName = ui.lineEditObjTexName;
    tileUIObjects.spinBoxObjTexIndex = ui.spinBoxObjTexIndex;
    tileUIObjects.checkBoxObjectAbovePlayer = ui.checkBoxObjectAbovePlayer;
    tileUIObjects.checkBoxTileCanSteppedOn = ui.checkBoxTileCanSteppedOn;
    tileUIObjects.checkBoxIsWallToClimb = ui.checkBoxIsWallToClimb;
    tileUIObjects.listWidgetMapTileTriggers = ui.listWidgetMapTileTriggers;
    tileUIObjects.pushButtonAddTileEvent = ui.pushButtonAddTileEvent;
    tileUIObjects.pushButtonEditTileEvent = ui.pushButtonEditTileEvent;
    tileUIObjects.pushButtonDeleteTileEvent = ui.pushButtonDeleteTileEvent;
    m_tileTabComponent.initializeUIObjects(tileUIObjects);
    //MonsterZoneTab Component initialization
    MainForm_MonsterZoneTabComponent_Objects monsterZoneUIObjects;
    monsterZoneUIObjects.glComponent = &m_glComponent;
    monsterZoneUIObjects.tableWidgetMonsterZone = ui.tableWidgetMonsterZone;
    monsterZoneUIObjects.pushButtonAddMonsterZone = ui.pushButtonAddMonsterZone;
    monsterZoneUIObjects.pushButtonEditMonsterZone = ui.pushButtonEditMonsterZone;
    monsterZoneUIObjects.pushButtonDeleteMonsterZone = ui.pushButtonDeleteMonsterZone;
    m_monsterZoneTabComponent.initializeUIObjects(monsterZoneUIObjects);
    m_monsterZoneTabComponent.setMonsterStores(m_controller.getMonsterStores());
    m_monsterZoneTabComponent.setResourcesPath(m_controller.getResourcesPath());
    //TextureListTab Component initialization
    MainForm_TextureListTabComponent_Objects textureListUIObjects;
    textureListUIObjects.glComponent = &m_glComponent;
    textureListUIObjects.listWidgetTextures = ui.listWidgetTextures;
    textureListUIObjects.pushButtonAddTexture = ui.pushButtonAddTexture;
    textureListUIObjects.pushButtonEditTexture = ui.pushButtonEditTexture;
    textureListUIObjects.pushButtonDeleteTexture = ui.pushButtonDeleteTexture;
    m_textureListTabComponent.initializeUIObjects(textureListUIObjects);
    //TextureSelection Component initialization
    MainForm_TextureSelectionComponent_Objects textureSelectionUIObjects;
    textureSelectionUIObjects.glComponent = &m_glComponent;
    textureSelectionUIObjects.comboBoxTexture = ui.comboBoxTexture;
    textureSelectionUIObjects.labelSelectedTexture = ui.labelSelectedTexture;
    textureSelectionUIObjects.pushButtonSelectedTextureClear = ui.pushButtonSelectedTextureClear;
    textureSelectionUIObjects.labelSelectedObject = ui.labelSelectedObject;
    textureSelectionUIObjects.pushButtonSelectedObjectClear = ui.pushButtonSelectedObjectClear;
    textureSelectionUIObjects.labelImageTexture = ui.labelImageTexture;
    m_textureSelectionComponent.initializeUIObjects(textureSelectionUIObjects);

    comboBoxToolbarMonsterZone = std::make_shared<QComboBox>(this);
    ui.toolBar->insertWidget(ui.action_ApplyMonsterZone, comboBoxToolbarMonsterZone.get());

    connectUIActions();

    //Generate a test map
    if (!m_controller.createMap(20, 20)) {
        ErrorMessage::show(m_controller.getLastError());
        exit(1);
    }
    auto map { m_controller.getMap() };
    m_glComponent.setCurrentMap(map);
    refreshRecentMapsMenu();
    refreshTextureList();
    refreshMonsterZones();
    m_mapTabComponent.reset();
}

void MainForm::connectUIActions()
{
    connect(ui.action_Quit, &QAction::triggered, this, &MainForm::close);
    connect(ui.action_About, &QAction::triggered, this, &MainForm::action_About_Click);
    connect(ui.action_Open, &QAction::triggered, this, &MainForm::action_Open_Click);
    connect(ui.action_RecentMap1, &QAction::triggered, this, &MainForm::action_OpenRecentMap_Click);
    connect(ui.action_RecentMap2, &QAction::triggered, this, &MainForm::action_OpenRecentMap_Click);
    connect(ui.action_RecentMap3, &QAction::triggered, this, &MainForm::action_OpenRecentMap_Click);
    connect(ui.action_RecentMap4, &QAction::triggered, this, &MainForm::action_OpenRecentMap_Click);
    connect(ui.action_RecentMap5, &QAction::triggered, this, &MainForm::action_OpenRecentMap_Click);
    connect(ui.action_Save, &QAction::triggered, this, &MainForm::action_Save_Click);
    connect(ui.action_SaveAs, &QAction::triggered, this, &MainForm::action_SaveAs_Click);
    connect(ui.action_LightTheme, &QAction::triggered, this, &MainForm::action_LightTheme_Click);
    connect(ui.action_DarkTheme, &QAction::triggered, this, &MainForm::action_DarkTheme_Click);
    connect(ui.action_DisplayGrid, &QAction::triggered, this, &MainForm::action_DisplayGrid_Click);
    connect(ui.action_ItemStore, &QAction::triggered, this, &MainForm::action_ManageItemStore_Click);
    connect(ui.action_MonsterStore, &QAction::triggered, this, &MainForm::action_ManageMonsterStore_Click);
    connect(ui.action_Select, &QAction::triggered, this, &MainForm::action_SelectClick);
    connect(ui.action_MoveMap, &QAction::triggered, this, &MainForm::action_MoveMapClick);
    connect(ui.action_ApplyTexture, &QAction::triggered, this, &MainForm::action_ApplyTextureClick);
    connect(ui.action_ApplyObject, &QAction::triggered, this, &MainForm::action_ApplyObjectClick);
    connect(ui.action_EnableCanStep, &QAction::triggered, this, &MainForm::action_EnableCanStepClick);
    connect(ui.action_DisableCanStep, &QAction::triggered, this, &MainForm::action_DisableCanStepClick);
    connect(ui.action_ViewBorderMode, &QAction::triggered, this, &MainForm::action_ViewBorderModeClick);
    connect(ui.action_BlockLeftBorder, &QAction::triggered, this, &MainForm::action_BlockLeftBorderClick);
    connect(ui.action_BlockTopBorder, &QAction::triggered, this, &MainForm::action_BlockTopBorderClick);
    connect(ui.action_BlockRightBorder, &QAction::triggered, this, &MainForm::action_BlockRightBorderClick);
    connect(ui.action_BlockBottomBorder, &QAction::triggered, this, &MainForm::action_BlockBottomBorderClick);
    connect(ui.action_ClearBlockedBorders, &QAction::triggered, this, &MainForm::action_ClearBlockedBordersClick);
    m_glComponent.connectUIActions();
    m_mapTabComponent.connectUIActions();
    m_tileTabComponent.connectUIActions();
    m_monsterZoneTabComponent.connectUIActions();
    m_textureListTabComponent.connectUIActions();
    m_textureSelectionComponent.connectUIActions();
    connect(&m_glComponent, &MainForm_GLComponent::tileSelected, this, &MainForm::onTileSelected);
    connect(&m_textureListTabComponent, &MainForm_TextureListTabComponent::textureAdded, this, &MainForm::onTextureAdded);
    connect(&m_textureListTabComponent, &MainForm_TextureListTabComponent::textureUpdated, this, &MainForm::onTextureUpdated);
    connect(&m_textureListTabComponent, &MainForm_TextureListTabComponent::textureDeleted, this, &MainForm::onTextureDeleted);
}

void MainForm::action_Open_Click()
{
    ui.mapOpenGLWidget->stopAutoUpdate();
    QString fullFilePath { QFileDialog::getOpenFileName(this,
            tr("Open map"),
            "",
            tr("Map file (*.map)")) };
    if (fullFilePath != "") {
        openMap(fullFilePath.toStdString());
    }
    refreshWindowTitle();
    ui.mapOpenGLWidget->startAutoUpdate();
}

void MainForm::action_OpenRecentMap_Click()
{
    QAction *recentAction = qobject_cast<QAction *>(sender());
    string filename = recentAction->text().toStdString();
    ui.mapOpenGLWidget->stopAutoUpdate();
    openMap(filename);
    refreshWindowTitle();
    ui.mapOpenGLWidget->startAutoUpdate();
}

void MainForm::action_Save_Click()
{
    if (m_currentFilePath == "") {
        action_SaveAs_Click();
    }
    else {
        saveMap(m_currentFilePath);
    }
}

void MainForm::action_SaveAs_Click()
{
    ui.mapOpenGLWidget->stopAutoUpdate();
    QString filter = "Map Files (*.map)";
    QString fullFilePath { QFileDialog::getSaveFileName(this,
            tr("Save map"),
            "",
            filter, &filter) };
    if (fullFilePath != "") {
        m_currentFilePath = fullFilePath.toStdString();
        saveMap(m_currentFilePath);
        addNewRecentMap(m_currentFilePath);
    }
    refreshWindowTitle();
    ui.mapOpenGLWidget->startAutoUpdate();
}

MainForm::~MainForm()
{
}

void MainForm::functionAfterShown()
{
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
}

bool MainForm::event(QEvent *event)
{
    const bool ret_val = QMainWindow::event(event);
    if(!m_functionAfterShownCalled && event->type() == QEvent::Paint)
    {
        m_functionAfterShownCalled = true;
        functionAfterShown();
    }
    return ret_val;
}

void MainForm::action_About_Click()
{
    AboutBoxForm aboutBoxForm(this);
    aboutBoxForm.exec();
}

void MainForm::action_LightTheme_Click()
{
    ConfigurationManager configManager(m_controller.getUserConfigFolder() + "config.json");
    if (configManager.load()) {
        configManager.setStringValue(MainForm::THEME_PATH, "");
        setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
        if (!configManager.save()) {
            ErrorMessage::show("An error occurred while saving the configuration file.",
                    configManager.getLastError());
        }
    }
    else {
        ErrorMessage::show("An error occurred while loading the configuration file.",
                configManager.getLastError());
    }

}

void MainForm::action_DarkTheme_Click()
{
    ConfigurationManager configManager(m_controller.getUserConfigFolder() + "config.json");
    if (configManager.load()) {
        configManager.setStringValue(MainForm::THEME_PATH, "Dark");
        setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
        if (!configManager.save()) {
            ErrorMessage::show("An error occurred while saving the configuration file.",
                    configManager.getLastError());
        }
    }
    else {
        ErrorMessage::show("An error occurred while loading the configuration file.",
                configManager.getLastError());
    }
}

void MainForm::action_DisplayGrid_Click()
{
    ui.mapOpenGLWidget->setGridEnabled(ui.action_DisplayGrid->isChecked());
}

void MainForm::action_ManageItemStore_Click()
{
    ManageItemStoreForm manageItemStoreForm(this, m_controller.getResourcesPath(), m_controller.getUserConfigFolder());
    manageItemStoreForm.exec();
}

void MainForm::action_ManageMonsterStore_Click()
{
    ManageMonsterStoreForm manageMonsterStoreForm(this, m_controller.getResourcesPath(), m_controller.getUserConfigFolder());
    manageMonsterStoreForm.exec();
    m_controller.loadConfiguredMonsterStores();
}

void MainForm::action_SelectClick()
{
    m_glComponent.setSelectionMode(SelectionMode::Select);
}

void MainForm::action_MoveMapClick()
{
    m_glComponent.setSelectionMode(SelectionMode::MoveMap);
}

void MainForm::action_ApplyTextureClick()
{
    m_glComponent.setSelectionMode(SelectionMode::ApplyTexture);
}

void MainForm::action_ApplyObjectClick()
{
    m_glComponent.setSelectionMode(SelectionMode::ApplyObject);
}

void MainForm::action_EnableCanStepClick()
{
    m_glComponent.setSelectionMode(SelectionMode::EnableCanStep);
}

void MainForm::action_DisableCanStepClick()
{
    m_glComponent.setSelectionMode(SelectionMode::DisableCanStep);
}

void MainForm::action_ViewBorderModeClick()
{
    m_glComponent.setSelectionMode(SelectionMode::ViewBorderMode);
}

void MainForm::action_BlockLeftBorderClick()
{
    m_glComponent.setSelectionMode(SelectionMode::BlockBorderLeft);
}

void MainForm::action_BlockTopBorderClick()
{
    m_glComponent.setSelectionMode(SelectionMode::BlockBorderTop);
}

void MainForm::action_BlockRightBorderClick()
{
    m_glComponent.setSelectionMode(SelectionMode::BlockBorderRight);
}
void MainForm::action_BlockBottomBorderClick()
{
    m_glComponent.setSelectionMode(SelectionMode::BlockBorderBottom);
}

void MainForm::action_ClearBlockedBordersClick()
{
    m_glComponent.setSelectionMode(SelectionMode::ClearBlockedBorders);
}

void MainForm::openMap(const std::string &filePath)
{
    GameMapStorage mapStorage;
    try {
        mapStorage.loadMap(filePath, m_controller.getMap());
    }
    catch(invalid_argument &err) {
        ErrorMessage::show(err.what());
        return;
    }
    catch(runtime_error &err) {
        ErrorMessage::show(err.what());
        return;
    }

    m_currentFilePath = filePath;
    addNewRecentMap(m_currentFilePath);
    m_glComponent.setCurrentMap(m_controller.getMap());
    m_glComponent.resetMapMovePosition();
    refreshTextureList();
    refreshMonsterZones();
    m_tileTabComponent.reset();
    m_mapTabComponent.reset();
}

void MainForm::saveMap(const std::string &filePath)
{
    ofstream ofs(filePath, ofstream::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << *m_controller.getMap();
}

void MainForm::refreshWindowTitle()
{
    if (m_currentFilePath == "") {
        setWindowTitle("MapEditor");
    }
    else {
        setWindowTitle(fmt::format("MapEditor - {0}", m_currentFilePath).c_str());
    }
}

void MainForm::refreshRecentMapsMenu()
{
    auto recents = vector<string> {};
    ConfigurationManager configManager(m_controller.getUserConfigFolder() + "config.json");
    if (configManager.load()) {
        recents = configManager.getVectorOfStringValue(MainForm::RECENT_MAPS);
    }
    else {
        ErrorMessage::show("An error occurred while loading the configuration file.",
                configManager.getLastError());
        return;
    }

    if (recents.size() > 5) {
        recents.resize(5);
    }
    ui.menu_RecentMaps->setEnabled(!recents.empty());
    vector<QAction *> actions {
        ui.action_RecentMap1,
            ui.action_RecentMap2,
            ui.action_RecentMap3,
            ui.action_RecentMap4,
            ui.action_RecentMap5
    };
    for(size_t i = 0; i < recents.size(); i++) {
        actions[i]->setVisible(true);
        actions[i]->setText(recents[i].c_str());
    }
}

void MainForm::addNewRecentMap(const std::string &filePath)
{
    auto recents = vector<string> {};
    //Load existing recent maps
    ConfigurationManager configManager(m_controller.getUserConfigFolder() + "config.json");
    if (configManager.load()) {
        recents = configManager.getVectorOfStringValue(MainForm::RECENT_MAPS);
    }
    else {
        ErrorMessage::show("An error occurred while loading the configuration file.",
                configManager.getLastError());
        return;
    }
    //Scan to find the currentMap, if found remove it from the list
    auto iter = std::find(recents.begin(), recents.end(), filePath);
    if (iter != recents.end()) {
        recents.erase(iter);
    }
    //Add it at the beginning of the vector
    recents.insert(recents.begin(), filePath);
    if (recents.size() > 5) {
        recents.resize(5);
    }
    configManager.setVectorOfStringValue(MainForm::RECENT_MAPS, recents);
    if (!configManager.save()) {
        ErrorMessage::show("An error occurred while saving the configuration file.",
                configManager.getLastError());
        return;
    }
    refreshRecentMapsMenu();
}

void MainForm::setAppStylesheet(const std::string &style)
{
    /*
       The Dark theme comes has been built by Colin Duquesnoy
       Github page : https://github.com/ColinDuquesnoy
       Project page : https://github.com/ColinDuquesnoy/QDarkStyleSheet
       */
    ui.action_LightTheme->setChecked(false);
    ui.action_DarkTheme->setChecked(false);
    QString styleSheet = "";
    if (style == "Dark") {
        QFile file(fmt::format("{0}/darkstyle/darkstyle.qss", m_controller.getResourcesPath()).c_str());
        file.open(QFile::ReadOnly);
        styleSheet = QLatin1String(file.readAll());
        ui.action_DarkTheme->setChecked(true);
    }
    else {
        ui.action_LightTheme->setChecked(true);
    }
    this->setStyleSheet(styleSheet);
    m_textureListTabComponent.setStyleSheet(styleSheet);
    m_tileTabComponent.setStyleSheet(styleSheet);
}

void MainForm::resizeEvent(QResizeEvent *)
{
    ui.mapOpenGLWidget->resizeGL(ui.mapOpenGLWidget->width(), ui.mapOpenGLWidget->height());
}

void MainForm::onTileSelected(MapTile *, Point<>)
{
    ui.toolBox->setCurrentWidget(ui.page_TileProperties);
}

void MainForm::onTextureAdded(TextureInfo textureInfo)
{
    if (!m_controller.addTexture(textureInfo)) {
        ErrorMessage::show(m_controller.getLastError());
    }
    refreshTextureList();
}

void MainForm::onTextureUpdated(const std::string &name, TextureInfo textureInfo)
{
    if (!m_controller.replaceTexture(name, textureInfo)) {
        ErrorMessage::show(m_controller.getLastError());
    }
    refreshTextureList();
}

void MainForm::onTextureDeleted(const std::string &name)
{
    if (!m_controller.removeTexture(name)) {
        ErrorMessage::show(m_controller.getLastError());
    }
    refreshTextureList();
}

void MainForm::refreshTextureList()
{
    m_textureListTabComponent.refreshTextureList();
    m_textureSelectionComponent.refreshTextureList();
    m_glComponent.reloadTextures();
}

void MainForm::refreshMonsterZones()
{
    m_monsterZoneTabComponent.refreshMonsterZones();
}
