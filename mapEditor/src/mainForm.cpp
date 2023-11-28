#include "mainForm.hpp"
#include <qcombobox.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qslider.h>
#include <qtabwidget.h>
#include <qtimer.h>
#include <QtCore/qfile.h>
#include <fmt/format.h>
#include <algorithm>
#include <fstream>
#include <memory>
#include <boost/algorithm/string.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>
#include "aboutBoxForm.hpp"
#include "components/debugInfoDockWidget.hpp"
#include "configurationManager.hpp"
#include "errorMessage.hpp"
#include "gameMapStorage.hpp"
#include "manageItemStoreForm.hpp"
#include "manageMonsterStoreForm.hpp"
#include "mapTile.hpp"
#include "mapView.hpp"
#include "monsterZoneDTO.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include "selectionMode.hpp"
#include "specialFolders.hpp"
#include "textureDTO.hpp"
#include "textureInfo.hpp"

using commoneditor::ui::ErrorMessage;
using commoneditor::ui::TextureDTO;
using mapeditor::controllers::MapTileDTO;
using mapeditor::controllers::MonsterZoneDTO;
using thewarrior::storage::ConfigurationManager;
using thewarrior::storage::GameMapStorage;

const char MainForm::THEME_PATH[] { "Display.Theme" };
const char MainForm::RECENT_MAPS[] { "Map.Recents" };

MainForm::MainForm(QWidget *parent,
        const std::string &currentFilePath)
    : QMainWindow(parent),
    ui(Ui::MainForm()) {
    ui.setupUi(this);

    m_controller.initializeExecutablePath();
    m_controller.initializeResourcesPath();
    m_controller.initializeUserConfigFolder();

    // Check if the user configuration folder exist
    const auto &userConfigFolder = m_controller.getUserConfigFolder();
    if (!boost::filesystem::exists(userConfigFolder)) {
        if (!boost::filesystem::create_directory(userConfigFolder)) {
            ErrorMessage::show(fmt::format("Unable to create the folder {0}", userConfigFolder), "");
            exit(1);
        }
    }

    // Check if the configuration file exist
    ConfigurationManager configManager(userConfigFolder + "config.json");
    if (!configManager.fileExists()) {
        // Try to create a default configuration
        if (!configManager.save()) {
            ErrorMessage::show("An error occurred while creation a default the configuration file.",
                    configManager.getLastError());
        }
    }
    if (configManager.load()) {
        setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
    } else {
        ErrorMessage::show("An error occurred while loading the configuration file.",
                configManager.getLastError());
    }

    m_controller.loadConfiguredMonsterStores();

    m_glComponent.initializeUIObjects(ui.mapOpenGLWidget);
    m_glComponent.setResourcesPath(m_controller.getResourcesPath());
    m_glComponent.setSelectionMode(SelectionMode::Select);

    m_controller.setGLComponentController(m_glComponent.getControllerPtr());
    componentInitialization();
    labelToolbarMonsterZoneColor = std::make_shared<QLabel>(this);
    labelToolbarMonsterZoneColor->setFixedWidth(40);
    labelToolbarMonsterZoneColor->setFixedHeight(32);
    ui.toolBar->insertWidget(ui.action_ApplyMonsterZone, labelToolbarMonsterZoneColor.get());
    comboBoxToolbarMonsterZone = std::make_shared<QComboBox>(this);
    ui.toolBar->insertWidget(ui.action_ApplyMonsterZone, comboBoxToolbarMonsterZone.get());
    labelToolbarZoom = std::make_shared<QLabel>(this);
    labelToolbarZoom->setText("Zoom: ");
    labelToolbarZoom->setMargin(10);
    ui.toolBar->insertWidget(nullptr, labelToolbarZoom.get());
    sliderZoom = std::make_shared<QSlider>(Qt::Horizontal, this);
    sliderZoom->setFixedWidth(160);
    sliderZoom->setMinimum(30);
    sliderZoom->setMaximum(150);
    sliderZoom->setValue(100);
    ui.toolBar->insertWidget(nullptr, sliderZoom.get());
    labelToolbarZoomValue = std::make_shared<QLabel>(this);
    labelToolbarZoomValue->setText("100%");
    ui.toolBar->insertWidget(nullptr, labelToolbarZoomValue.get());

    connectUIActions();

    if (!m_controller.createMap(20, 20)) {
        ErrorMessage::show(m_controller.getLastError());
        exit(1);
    }
    if (!currentFilePath.empty()) {
        ui.mapOpenGLWidget->stopAutoUpdate();
        openMap(currentFilePath);
        refreshWindowTitle();
        ui.mapOpenGLWidget->startAutoUpdate();
    } else {
        auto map { m_controller.getMap() };
        m_glComponent.setCurrentMap(map);
    }
    refreshRecentMapsMenu();
    refreshUndoControls();
    refreshTextureList();
    refreshMonsterZones();
    m_mapPropsComponent->reset();
    m_debugInfoDockWidget->hide();
    action_SelectClick();
    tabWidgetMapViewChanged(static_cast<int>(MapView::Standard));
}

void MainForm::componentInitialization() {
    m_mapPropsComponent = std::make_shared<MapPropsComponent>(this,
            &m_glComponent);
    ui.toolBox->addItem(m_mapPropsComponent.get(), "Map properties");

    m_tilePropsComponent = std::make_shared<TilePropsComponent>(this,
            &m_glComponent,
            m_glComponent.getControllerPtr());
    ui.toolBox->addItem(m_tilePropsComponent.get(), "Tile properties");

    m_textureListComponent = std::make_shared<TextureListComponent>(this,
            &m_glComponent);
    ui.toolBox->addItem(m_textureListComponent.get(), "Texture list");

    m_monsterZoneListComponent = std::make_shared<MonsterZoneListComponent>(this,
            &m_glComponent);
    m_monsterZoneListComponent->setMonsterStores(m_controller.getMonsterStores());
    m_monsterZoneListComponent->setResourcesPath(m_controller.getResourcesPath());
    ui.toolBox->addItem(m_monsterZoneListComponent.get(), "Monster zones");
    ui.toolBox->removeItem(0);

    m_textureSelectionDockWidget = std::make_shared<TextureSelectionDockWidget>(this,
            &m_glComponent);
    this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea,
            m_textureSelectionDockWidget.get());

    m_debugInfoDockWidget = std::make_shared<DebugInfoDockWidget>(this,
            ui.mapOpenGLWidget);
    this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea,
            m_debugInfoDockWidget.get());
    m_debugInfoDockWidget->hide();
}

void MainForm::connectUIActions() {
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
    connect(ui.actionView_MapConfiguration, &QAction::triggered, this, &MainForm::toggleViewMapConfiguration);
    connect(ui.actionView_TextureSelection, &QAction::triggered, this, &MainForm::toggleViewTextureSelection);
    connect(ui.actionView_DebuggingInfo, &QAction::triggered, this, &MainForm::toggleViewDebuggingInfo);
    connect(ui.action_LightTheme, &QAction::triggered, this, &MainForm::action_LightTheme_Click);
    connect(ui.action_DarkTheme, &QAction::triggered, this, &MainForm::action_DarkTheme_Click);
    connect(ui.action_DisplayGrid, &QAction::triggered, this, &MainForm::action_DisplayGrid_Click);
    connect(ui.action_ItemStore, &QAction::triggered, this, &MainForm::action_ManageItemStore_Click);
    connect(ui.action_MonsterStore, &QAction::triggered, this, &MainForm::action_ManageMonsterStore_Click);
    connect(ui.action_Select, &QAction::triggered, this, &MainForm::action_SelectClick);
    connect(ui.action_MoveMap, &QAction::triggered, this, &MainForm::action_MoveMapClick);
    connect(ui.action_PickerTool, &QAction::triggered, this, &MainForm::action_PickerToolClick);
    connect(ui.action_Undo, &QAction::triggered, this, &MainForm::action_UndoClick);
    connect(ui.action_Redo, &QAction::triggered, this, &MainForm::action_RedoClick);
    connect(ui.action_ApplyTexture, &QAction::triggered, this, &MainForm::action_ApplyTextureClick);
    connect(ui.action_ApplyObject, &QAction::triggered, this, &MainForm::action_ApplyObjectClick);
    connect(ui.action_EnableCanStep, &QAction::triggered, this, &MainForm::action_EnableCanStepClick);
    connect(ui.action_DisableCanStep, &QAction::triggered, this, &MainForm::action_DisableCanStepClick);
    connect(ui.action_BlockLeftBorder, &QAction::triggered, this, &MainForm::action_BlockLeftBorderClick);
    connect(ui.action_BlockTopBorder, &QAction::triggered, this, &MainForm::action_BlockTopBorderClick);
    connect(ui.action_BlockRightBorder, &QAction::triggered, this, &MainForm::action_BlockRightBorderClick);
    connect(ui.action_BlockBottomBorder, &QAction::triggered, this, &MainForm::action_BlockBottomBorderClick);
    connect(ui.action_ClearBlockedBorders, &QAction::triggered, this, &MainForm::action_ClearBlockedBordersClick);
    connect(comboBoxToolbarMonsterZone.get(), static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainForm::onComboBoxToolbarMonsterZoneCurrentIndexChanged);
    connect(ui.action_ApplyMonsterZone, &QAction::triggered, this, &MainForm::action_ApplyMonsterZone);
    connect(ui.action_ClearMonsterZone, &QAction::triggered, this, &MainForm::action_ClearMonsterZone);
    connect(sliderZoom.get(), &QSlider::valueChanged, this, &MainForm::sliderZoomValueChanged);
    connect(ui.tabWidgetMapView, &QTabWidget::currentChanged, this, &MainForm::tabWidgetMapViewChanged);
    connect(ui.dockWidgetMapConfig, &QDockWidget::visibilityChanged, this, &MainForm::widgetMapConfigVisibilityChanged);
    connect(m_textureSelectionDockWidget.get(), &QDockWidget::visibilityChanged, this, &MainForm::widgetTextureSelectionVisibilityChanged);
    connect(m_debugInfoDockWidget.get(), &QDockWidget::visibilityChanged, this, &MainForm::widgetDebugInfoVisibilityChanged);
    m_glComponent.connectUIActions();
    connect(&m_glComponent, &MainForm_GLComponent::tileSelected, this, &MainForm::onTileSelected);
    connect(&m_glComponent, &MainForm_GLComponent::editHistoryChanged, this, &MainForm::onEditHistoryChanged);
    connect(m_textureListComponent.get(), &TextureListComponent::textureAdded, this, &MainForm::onTextureAdded);
    connect(m_textureListComponent.get(), &TextureListComponent::textureUpdated, this, &MainForm::onTextureUpdated);
    connect(m_textureListComponent.get(), &TextureListComponent::textureDeleted, this, &MainForm::onTextureDeleted);
    connect(m_monsterZoneListComponent.get(), &MonsterZoneListComponent::monsterZoneAdded, this, &MainForm::onMonsterZoneAdded);
    connect(m_monsterZoneListComponent.get(), &MonsterZoneListComponent::monsterZoneUpdated, this, &MainForm::onMonsterZoneUpdated);
    connect(m_monsterZoneListComponent.get(), &MonsterZoneListComponent::monsterZoneDeleted, this, &MainForm::onMonsterZoneDeleted);
    connect(m_monsterZoneListComponent.get(), &MonsterZoneListComponent::useOnlyOneMonsterZoneChanged, this, &MainForm::useOnlyOneMonsterZoneChanged);
}

void MainForm::action_Open_Click() {
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

void MainForm::action_OpenRecentMap_Click() {
    QAction *recentAction = qobject_cast<QAction *>(sender());
    std::string filename = recentAction->text().toStdString();
    ui.mapOpenGLWidget->stopAutoUpdate();
    openMap(filename);
    refreshWindowTitle();
    ui.mapOpenGLWidget->startAutoUpdate();
}

void MainForm::action_Save_Click() {
    if (m_currentFilePath == "") {
        action_SaveAs_Click();
    } else {
        saveMap(m_currentFilePath);
    }
}

void MainForm::action_SaveAs_Click() {
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

MainForm::~MainForm() {
}

void MainForm::functionAfterShown() {
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
}

bool MainForm::event(QEvent *event) {
    const bool ret_val = QMainWindow::event(event);
    if (!m_functionAfterShownCalled && event->type() == QEvent::Paint) {
        m_functionAfterShownCalled = true;
        functionAfterShown();
    }
    return ret_val;
}

void MainForm::action_About_Click() {
    AboutBoxForm aboutBoxForm(this);
    aboutBoxForm.exec();
}

void MainForm::toggleViewMapConfiguration() {
    ui.dockWidgetMapConfig->setVisible(!ui.dockWidgetMapConfig->isVisible());
}

void MainForm::toggleViewTextureSelection() {
    m_textureSelectionDockWidget->setVisible(!m_textureSelectionDockWidget->isVisible());
}

void MainForm::toggleViewDebuggingInfo() {
    m_debugInfoDockWidget->setVisible(!m_debugInfoDockWidget->isVisible());
}

void MainForm::action_LightTheme_Click() {
    ConfigurationManager configManager(m_controller.getUserConfigFolder() + "config.json");
    if (configManager.load()) {
        configManager.setStringValue(MainForm::THEME_PATH, "");
        setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
        if (!configManager.save()) {
            ErrorMessage::show("An error occurred while saving the configuration file.",
                    configManager.getLastError());
        }
    } else {
        ErrorMessage::show("An error occurred while loading the configuration file.",
                configManager.getLastError());
    }
}

void MainForm::action_DarkTheme_Click() {
    ConfigurationManager configManager(m_controller.getUserConfigFolder() + "config.json");
    if (configManager.load()) {
        configManager.setStringValue(MainForm::THEME_PATH, "Dark");
        setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
        if (!configManager.save()) {
            ErrorMessage::show("An error occurred while saving the configuration file.",
                    configManager.getLastError());
        }
    } else {
        ErrorMessage::show("An error occurred while loading the configuration file.",
                configManager.getLastError());
    }
}

void MainForm::action_DisplayGrid_Click() {
    ui.mapOpenGLWidget->setGridEnabled(ui.action_DisplayGrid->isChecked());
}

void MainForm::action_ManageItemStore_Click() {
    ManageItemStoreForm manageItemStoreForm(this, m_controller.getResourcesPath(), m_controller.getUserConfigFolder());
    manageItemStoreForm.exec();
}

void MainForm::action_ManageMonsterStore_Click() {
    ManageMonsterStoreForm manageMonsterStoreForm(this, m_controller.getResourcesPath(), m_controller.getUserConfigFolder());
    manageMonsterStoreForm.exec();
    m_controller.loadConfiguredMonsterStores();
}

void MainForm::setActiveToolbarActionChecked(SelectionMode mode) {
    ui.action_Select->setChecked(mode == SelectionMode::Select);
    ui.action_MoveMap->setChecked(mode == SelectionMode::MoveMap);
    ui.action_PickerTool->setChecked(mode == SelectionMode::PickerTool);
}

void MainForm::action_SelectClick() {
    m_glComponent.setSelectionMode(SelectionMode::Select);
    setActiveToolbarActionChecked(SelectionMode::Select);
}

void MainForm::action_MoveMapClick() {
    m_glComponent.setSelectionMode(SelectionMode::MoveMap);
    setActiveToolbarActionChecked(SelectionMode::MoveMap);
}

void MainForm::action_PickerToolClick() {
    m_glComponent.setSelectionMode(SelectionMode::PickerTool);
    setActiveToolbarActionChecked(SelectionMode::PickerTool);
}

void MainForm::action_UndoClick() {
    m_glComponent.undo();
    refreshTextureList();
    refreshMonsterZones();
}

void MainForm::action_RedoClick() {
    m_glComponent.redo();
    refreshTextureList();
    refreshMonsterZones();
}

void MainForm::action_ApplyTextureClick() {
    m_glComponent.applyTexture();
}

void MainForm::action_ApplyObjectClick() {
    m_glComponent.applyObject();
}

void MainForm::action_EnableCanStepClick() {
    m_glComponent.applyCanStep(true);
    ui.tabWidgetMapView->setCurrentIndex(1);
}

void MainForm::action_DisableCanStepClick() {
    m_glComponent.applyCanStep(false);
    ui.tabWidgetMapView->setCurrentIndex(1);
}

void MainForm::action_BlockLeftBorderClick() {
    m_glComponent.addMoveDenyTrigger("MoveLeftPressed");
    ui.tabWidgetMapView->setCurrentIndex(2);
}

void MainForm::action_BlockTopBorderClick() {
    m_glComponent.addMoveDenyTrigger("MoveUpPressed");
    ui.tabWidgetMapView->setCurrentIndex(2);
}

void MainForm::action_BlockRightBorderClick() {
    m_glComponent.addMoveDenyTrigger("MoveRightPressed");
    ui.tabWidgetMapView->setCurrentIndex(2);
}

void MainForm::action_BlockBottomBorderClick() {
    m_glComponent.addMoveDenyTrigger("MoveDownPressed");
    ui.tabWidgetMapView->setCurrentIndex(2);
}

void MainForm::action_ClearBlockedBordersClick() {
    m_glComponent.clearMoveDenyTriggers();
    ui.tabWidgetMapView->setCurrentIndex(2);
}

void MainForm::onComboBoxToolbarMonsterZoneCurrentIndexChanged() {
    const auto zoneName = comboBoxToolbarMonsterZone->currentText().toStdString();
    const auto colorValue = m_monsterZoneListComponent->getMonsterZoneColor(zoneName);
    const auto defaultStyle = "margin-right: 8px; border-radius: 5px; border: 1px solid black";
    if (!colorValue.empty()) {
        labelToolbarMonsterZoneColor->setStyleSheet(fmt::format("background-color: {0}; {1}",
                    colorValue,
                    defaultStyle).c_str());
    } else {
        labelToolbarMonsterZoneColor->setStyleSheet(defaultStyle);
    }
    m_glComponent.setLastSelectedMonsterZone(comboBoxToolbarMonsterZone->currentIndex());
}

void MainForm::action_ApplyMonsterZone() {
    m_glComponent.applyMonsterZone();
    ui.tabWidgetMapView->setCurrentIndex(3);
}

void MainForm::action_ClearMonsterZone() {
    m_glComponent.clearMonsterZone();
    ui.tabWidgetMapView->setCurrentIndex(3);
}

void MainForm::sliderZoomValueChanged(int value) {
    labelToolbarZoomValue->setText(fmt::format("{0}%", value).c_str());
    ui.mapOpenGLWidget->setZoom(value);
}

void MainForm::tabWidgetMapViewChanged(int index) {
    switch (index) {
        case 0:
            m_glComponent.setMapView(MapView::Standard);
            break;
        case 1:
            m_glComponent.setMapView(MapView::CanStep);
            break;
        case 2:
            m_glComponent.setMapView(MapView::BlockedBorders);
            break;
        case 3:
            m_glComponent.setMapView(MapView::MonsterZones);
            break;
    }
}

void MainForm::openMap(const std::string &filePath) {
    GameMapStorage mapStorage;
    try {
        mapStorage.loadMap(filePath, m_controller.getMap());
    }
    catch(std::invalid_argument &err) {
        ErrorMessage::show(err.what());
        return;
    }
    catch(std::runtime_error &err) {
        ErrorMessage::show(err.what());
        return;
    }

    m_currentFilePath = filePath;
    addNewRecentMap(m_currentFilePath);
    m_glComponent.setCurrentMap(m_controller.getMap());
    m_glComponent.resetMapMovePosition();
    refreshTextureList();
    refreshMonsterZones();
    m_tilePropsComponent->reset();
    m_mapPropsComponent->reset();
}

void MainForm::saveMap(const std::string &filePath) {
    std::ofstream ofs(filePath, std::ofstream::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << *m_controller.getMap();
}

void MainForm::refreshWindowTitle() {
    if (m_currentFilePath == "") {
        setWindowTitle("MapEditor");
    } else {
        setWindowTitle(fmt::format("MapEditor - {0}", m_currentFilePath).c_str());
    }
}

void MainForm::refreshUndoControls() {
    size_t total = m_glComponent.getHistoryCount();
    size_t index = m_glComponent.getHistoryCurrentIndex();
    ui.action_Undo->setEnabled(total > 0 && index > 0);
    ui.action_Redo->setEnabled(total > 0 && index < total-1);
}

void MainForm::refreshRecentMapsMenu() {
    auto recents = std::vector<std::string> {};
    ConfigurationManager configManager(m_controller.getUserConfigFolder() + "config.json");
    if (configManager.load()) {
        recents = configManager.getVectorOfStringValue(MainForm::RECENT_MAPS);
    } else {
        ErrorMessage::show("An error occurred while loading the configuration file.",
                configManager.getLastError());
        return;
    }

    if (recents.size() > 5) {
        recents.resize(5);
    }
    ui.menu_RecentMaps->setEnabled(!recents.empty());
    std::vector<QAction *> actions {
        ui.action_RecentMap1,
            ui.action_RecentMap2,
            ui.action_RecentMap3,
            ui.action_RecentMap4,
            ui.action_RecentMap5
    };
    for (size_t i = 0; i < recents.size(); i++) {
        actions[i]->setVisible(true);
        actions[i]->setText(recents[i].c_str());
    }
}

void MainForm::addNewRecentMap(const std::string &filePath) {
    auto recents = std::vector<std::string> {};
    // Load existing recent maps
    ConfigurationManager configManager(m_controller.getUserConfigFolder() + "config.json");
    if (configManager.load()) {
        recents = configManager.getVectorOfStringValue(MainForm::RECENT_MAPS);
    } else {
        ErrorMessage::show("An error occurred while loading the configuration file.",
                configManager.getLastError());
        return;
    }
    // Scan to find the currentMap, if found remove it from the list
    auto iter = std::find(recents.begin(), recents.end(), filePath);
    if (iter != recents.end()) {
        recents.erase(iter);
    }
    // Add it at the beginning of the vector
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

void MainForm::setAppStylesheet(const std::string &style) {
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
    } else {
        ui.action_LightTheme->setChecked(true);
    }
    this->setStyleSheet(styleSheet);
}

void MainForm::resizeEvent(QResizeEvent *) {
    ui.mapOpenGLWidget->resizeGL(ui.mapOpenGLWidget->width(), ui.mapOpenGLWidget->height());
}

void MainForm::widgetMapConfigVisibilityChanged(bool visible) {
    ui.actionView_MapConfiguration->setChecked(visible);
}

void MainForm::widgetTextureSelectionVisibilityChanged(bool visible) {
    ui.actionView_TextureSelection->setChecked(visible);
}

void MainForm::widgetDebugInfoVisibilityChanged(bool visible) {
    ui.actionView_DebuggingInfo->setChecked(visible);
}

void MainForm::onTileSelected(std::vector<MapTileDTO>) {
    ui.toolBox->setCurrentWidget(m_tilePropsComponent.get());
}

void MainForm::onEditHistoryChanged() {
    refreshUndoControls();
}

void MainForm::onTextureAdded(TextureDTO textureDTO) {
    if (!m_controller.addTexture(textureDTO)) {
        ErrorMessage::show(m_controller.getLastError());
    }
    refreshTextureList();
    refreshUndoControls();
}

void MainForm::onTextureUpdated(const std::string &name, TextureDTO textureDTO) {
    if (!m_controller.replaceTexture(name, textureDTO)) {
        ErrorMessage::show(m_controller.getLastError());
    }
    refreshTextureList();
    refreshUndoControls();
}

void MainForm::onTextureDeleted(const std::string &name) {
    if (!m_controller.removeTexture(name)) {
        ErrorMessage::show(m_controller.getLastError());
    }
    refreshTextureList();
    refreshUndoControls();
}

void MainForm::refreshTextureList() {
    m_textureListComponent->refreshTextureList();
    m_textureSelectionDockWidget->refreshTextureList();
    m_glComponent.reloadTextures();
}

void MainForm::onMonsterZoneAdded(MonsterZoneDTO monsterZoneDTO) {
    if (m_controller.addMonsterZone(monsterZoneDTO)) {
        m_monsterZoneListComponent->confirmValidityOfOneMonsterZoneCheckBox();
    } else {
        ErrorMessage::show(m_controller.getLastError());
    }
    refreshMonsterZones();
    refreshUndoControls();
}

void MainForm::onMonsterZoneUpdated(const std::string &name, MonsterZoneDTO monsterZoneDTO) {
    if (!m_controller.replaceMonsterZone(name, monsterZoneDTO)) {
        ErrorMessage::show(m_controller.getLastError());
    }
    refreshMonsterZones();
    refreshUndoControls();
}

void MainForm::onMonsterZoneDeleted(const std::string &name) {
    if (m_controller.removeMonsterZone(name)) {
        m_monsterZoneListComponent->confirmValidityOfOneMonsterZoneCheckBox();
    } else {
        ErrorMessage::show(m_controller.getLastError());
    }
    refreshMonsterZones();
    refreshUndoControls();
}

void MainForm::refreshMonsterZones() {
    m_monsterZoneListComponent->refreshMonsterZones();
    int selectedComboBoxIndex = comboBoxToolbarMonsterZone->currentIndex();
    // Refresh Monster Zones toolbar combobox
    comboBoxToolbarMonsterZone->model()->removeRows(0, comboBoxToolbarMonsterZone->count());
    int i = 0;
    for (const auto &zone : m_monsterZoneListComponent->getMonsterZones()) {
        comboBoxToolbarMonsterZone->insertItem(i, zone.m_name.c_str());
        i++;
    }
    if (selectedComboBoxIndex != -1 && selectedComboBoxIndex < comboBoxToolbarMonsterZone->count()) {
        comboBoxToolbarMonsterZone->setCurrentIndex(selectedComboBoxIndex);
    }
    const int selectedMonsterZoneIndex = comboBoxToolbarMonsterZone->currentIndex();
    if (selectedMonsterZoneIndex != -1) {
        m_glComponent.setLastSelectedMonsterZone(selectedMonsterZoneIndex);
    } else {
        m_glComponent.clearLastSelectedMonsterZone();
    }
    toggleMonsterZoneAssignationControls();
}

void MainForm::toggleMonsterZoneAssignationControls() {
    bool active = !m_monsterZoneListComponent->isMonsterZonesEmpty() &&
        !m_monsterZoneListComponent->isOnlyOneMonsterZoneChecked();
    comboBoxToolbarMonsterZone->setEnabled(active);
    ui.action_ApplyMonsterZone->setEnabled(active);
    ui.action_ClearMonsterZone->setEnabled(active);
}

void MainForm::useOnlyOneMonsterZoneChanged(bool) {
    toggleMonsterZoneAssignationControls();
}

