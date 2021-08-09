#include "mainForm.hpp"
#include "aboutBoxForm.hpp"
#include "configurationManager.hpp"
#include "editMapTileTriggerForm.hpp"
#include "editTextureForm.hpp"
#include "editTileActionChangeMapPropertiesForm.hpp"
#include "errorMessage.hpp"
#include "specialFolders.hpp"
#include <QtCore/qfile.h>
#include <QtOpenGL/QtOpenGL>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <fstream>
#include <libgen.h>         // dirname
#include <linux/limits.h>   // PATH_MAX
#include <qtimer.h>
#include <unistd.h>         // readlink
using namespace std;

MainForm::MainForm(QWidget *parent)
	: QMainWindow(parent),
	  ui(Ui::MainForm()),
	  currentFilePath(""),
	  functionAfterShownCalled(false), 
	  executablePath(""),
	  resourcesPath("")
{
	ui.setupUi(this);
	glComponent.initializeUIObjects(ui.mapOpenGLWidget);
	glComponent.setResourcesPath(getResourcesPath());
	glComponent.setSelectionMode(SelectionMode::Select);
	//MapTab Component initialization
	MainForm_MapTabComponent_Objects mapUIObjects;
	mapUIObjects.glComponent = &glComponent;
	mapUIObjects.lineEditMapWidth = ui.lineEditMapWidth;
	mapUIObjects.lineEditMapHeight = ui.lineEditMapHeight;
	mapUIObjects.spinBoxMapSizeTop = ui.spinBoxMapSizeTop;
	mapUIObjects.spinBoxMapSizeLeft = ui.spinBoxMapSizeLeft;
	mapUIObjects.spinBoxMapSizeRight = ui.spinBoxMapSizeRight;
	mapUIObjects.spinBoxMapSizeBottom = ui.spinBoxMapSizeBottom;
	mapUIObjects.pushButtonApplySizeChange = ui.pushButtonApplySizeChange;
	mapTabComponent.initializeUIObjects(mapUIObjects);
	//TileTab Component initialization
	MainForm_TileTabComponent_Objects tileUIObjects;
	tileUIObjects.glComponent = &glComponent;
	tileUIObjects.labelTileCoordXY = ui.labelTileCoordXY;
	tileUIObjects.lineEditTexName = ui.lineEditTexName;
	tileUIObjects.spinBoxTexIndex = ui.spinBoxTexIndex;
	tileUIObjects.lineEditObjTexName = ui.lineEditObjTexName;
	tileUIObjects.spinBoxObjTexIndex = ui.spinBoxObjTexIndex;
	tileUIObjects.checkBoxObjectAbovePlayer = ui.checkBoxObjectAbovePlayer;
	tileUIObjects.checkBoxTileCanSteppedOn = ui.checkBoxTileCanSteppedOn;
	tileTabComponent.initializeUIObjects(tileUIObjects);
	//TextureListTab Component initialization
	MainForm_TextureListTabComponent_Objects textureListUIObjects;
	textureListUIObjects.glComponent = &glComponent;
	textureListUIObjects.listWidgetTextures = ui.listWidgetTextures;
	textureListUIObjects.pushButtonAddTexture = ui.pushButtonAddTexture;
	textureListUIObjects.pushButtonEditTexture = ui.pushButtonEditTexture;
	textureListUIObjects.pushButtonDeleteTexture = ui.pushButtonDeleteTexture;
	textureListTabComponent.initializeUIObjects(textureListUIObjects);
	//TextureSelection Component initialization
	MainForm_TextureSelectionComponent_Objects textureSelectionUIObjects;
	textureSelectionUIObjects.glComponent = &glComponent;
	textureSelectionUIObjects.comboBoxTexture = ui.comboBoxTexture;
	textureSelectionUIObjects.labelSelectedTexture = ui.labelSelectedTexture;
	textureSelectionUIObjects.pushButtonSelectedTextureClear = ui.pushButtonSelectedTextureClear;
	textureSelectionUIObjects.labelSelectedObject = ui.labelSelectedObject;
	textureSelectionUIObjects.pushButtonSelectedObjectClear = ui.pushButtonSelectedObjectClear;
	textureSelectionUIObjects.labelImageTexture = ui.labelImageTexture;
	textureSelectionComponent.initializeUIObjects(textureSelectionUIObjects);
	connectUIActions();
	generateComboxItems();

	//Check if the user configuration folder exist
	userConfigFolder = SpecialFolders::getUserConfigDirectory();
	if (!boost::filesystem::exists(userConfigFolder)) {
		if (!boost::filesystem::create_directory(userConfigFolder)) {
			ErrorMessage::show(fmt::format("Unable to create the folder {0}", userConfigFolder), "");
			exit(1);
		}
	}

	//Check if the configuration file exist
	ConfigurationManager configManager(userConfigFolder + "config.json");
	setAppStylesheet(configManager.getStringValue(ConfigurationManager::THEME_PATH));

	//Generate a test map
	if (!controller.createMap(20, 20)) {
		ErrorMessage::show(controller.getLastError());
		exit(1);
	}
	auto map { controller.getMap() };
	glComponent.setCurrentMap(map);
	ui.lineEditMapWidth->setText(to_string(map->getWidth()).c_str());
	ui.lineEditMapHeight->setText(to_string(map->getHeight()).c_str());
	refreshRecentMapsMenu();
	refreshTextureList();
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
	connect(ui.action_Select, &QAction::triggered, this, &MainForm::action_SelectClick);
	connect(ui.action_MoveMap, &QAction::triggered, this, &MainForm::action_MoveMapClick);
	connect(ui.action_ApplyTexture, &QAction::triggered, this, &MainForm::action_ApplyTextureClick);
	connect(ui.action_ApplyObject, &QAction::triggered, this, &MainForm::action_ApplyObjectClick);
	connect(ui.action_EnableCanStep, &QAction::triggered, this, &MainForm::action_EnableCanStepClick);
	connect(ui.action_DisableCanStep, &QAction::triggered, this, &MainForm::action_DisableCanStepClick);
	/*connect(ui.pushButtonSelectedTextureClear, &QPushButton::clicked, this, &MainForm::onPushButtonSelectedTextureClearClick);
	connect(ui.pushButtonSelectedObjectClear, &QPushButton::clicked, this, &MainForm::onPushButtonSelectedObjectClearClick);
	connect(ui.labelImageTexture, &QClickableLabel::onMouseReleaseEvent, this, &MainForm::onLabelImageTextureMouseReleaseEvent);
	connect(ui.comboBoxTexture, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainForm::onComboBoxTextureCurrentIndexChanged);*/
	glComponent.connectUIActions();
	mapTabComponent.connectUIActions();
	tileTabComponent.connectUIActions();
	textureListTabComponent.connectUIActions();
	textureSelectionComponent.connectUIActions();
	connect(&glComponent, &MainForm_GLComponent::tileSelected, this, &MainForm::onTileSelected);
	connect(&textureListTabComponent, &MainForm_TextureListTabComponent::textureAdded, this, &MainForm::onTextureAdded);
	connect(&textureListTabComponent, &MainForm_TextureListTabComponent::textureUpdated, this, &MainForm::onTextureUpdated);
	connect(&textureListTabComponent, &MainForm_TextureListTabComponent::textureDeleted, this, &MainForm::onTextureDeleted);
	
}

void MainForm::generateComboxItems() 
{
	//comboBoxTileTrigger
	/*ui.comboBoxTileTrigger->model()->removeRows(0, ui.comboBoxTileTrigger->count());
	ui.comboBoxTileTrigger->insertItem(0, "None");
	ui.comboBoxTileTrigger->insertItem(1, "SteppedOn");
	ui.comboBoxTileTrigger->insertItem(2, "MoveUpPressed");
	ui.comboBoxTileTrigger->insertItem(3, "MoveDownPressed");
	ui.comboBoxTileTrigger->insertItem(4, "MoveLeftPressed");
	ui.comboBoxTileTrigger->insertItem(5, "MoveRightLeft");
	ui.comboBoxTileTrigger->insertItem(6, "ActionButtonPressed");
	//comboBoxTileCondition
	ui.comboBoxTileCondition->model()->removeRows(0, ui.comboBoxTileCondition->count());
	ui.comboBoxTileCondition->insertItem(0, "None");
	ui.comboBoxTileCondition->insertItem(1, "MustBeFacing");
	ui.comboBoxTileCondition->insertItem(2, "MustHaveItem");
	//comboBoxTileAction
	ui.comboBoxTileAction->model()->removeRows(0, ui.comboBoxTileAction->count());
	ui.comboBoxTileAction->insertItem(0, "None");
	ui.comboBoxTileAction->insertItem(1, "OpenChest");
	ui.comboBoxTileAction->insertItem(2, "ChangeMap");*/
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
	if (currentFilePath == "") {
		action_SaveAs_Click();
	}
	else {
		saveMap(currentFilePath);
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
		currentFilePath = fullFilePath.toStdString();
		saveMap(currentFilePath);
		addNewRecentMap(currentFilePath);
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

const std::string &MainForm::getExecutablePath() 
{
	if (executablePath.empty()) {
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		if (count != -1) {
			executablePath = dirname(result);
		}
	}
	return executablePath;
}

const std::string& MainForm::getResourcesPath() 
{
	if (resourcesPath.empty()) {
		resourcesPath = fmt::format("{0}/resources/", getExecutablePath());
	}
	return resourcesPath;
}

bool MainForm::event(QEvent *event)
{
    const bool ret_val = QMainWindow::event(event);
    if(!functionAfterShownCalled && event->type() == QEvent::Paint)
    {
        functionAfterShownCalled = true;
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
	ConfigurationManager configManager(userConfigFolder + "config.json");
	configManager.setStringValue(ConfigurationManager::THEME_PATH, "");
	setAppStylesheet(configManager.getStringValue(ConfigurationManager::THEME_PATH));
	if (!configManager.save()) {
		ErrorMessage::show("An error occurred while saving the configuration file.", 
						 configManager.getLastError());
	}
}

void MainForm::action_DarkTheme_Click()
{
	ConfigurationManager configManager(userConfigFolder + "config.json");
	configManager.setStringValue(ConfigurationManager::THEME_PATH, "Dark");
	setAppStylesheet(configManager.getStringValue(ConfigurationManager::THEME_PATH));
	if (!configManager.save()) {
		ErrorMessage::show("An error occurred while saving the configuration file.", 
						 configManager.getLastError());
	}
}

void MainForm::action_DisplayGrid_Click() 
{
	ui.mapOpenGLWidget->setGridEnabled(ui.action_DisplayGrid->isChecked());
}

void MainForm::action_SelectClick() 
{
	glComponent.setSelectionMode(SelectionMode::Select);
}

void MainForm::action_MoveMapClick() 
{
	glComponent.setSelectionMode(SelectionMode::MoveMap);
}

void MainForm::action_ApplyTextureClick() 
{
	glComponent.setSelectionMode(SelectionMode::ApplyTexture);
}

void MainForm::action_ApplyObjectClick() 
{
	glComponent.setSelectionMode(SelectionMode::ApplyObject);
}

void MainForm::action_EnableCanStepClick() 
{
	glComponent.setSelectionMode(SelectionMode::EnableCanStep);
}

void MainForm::action_DisableCanStepClick() 
{
	glComponent.setSelectionMode(SelectionMode::DisableCanStep);
}

void MainForm::openMap(const std::string &filePath) 
{
	try {
		ifstream ofs(filePath, ifstream::binary);
		boost::archive::binary_iarchive oa(ofs);
		oa >> *controller.getMap();
		currentFilePath = filePath;
		addNewRecentMap(currentFilePath);
		refreshTextureList();
	}
	catch(...) {
		ErrorMessage::show(fmt::format("Unable to open the map {0}", filePath));
	}
}

void MainForm::saveMap(const std::string &filePath) 
{
	ofstream ofs(filePath, ofstream::binary);
	boost::archive::binary_oarchive oa(ofs);
	oa << *controller.getMap();
}

void MainForm::refreshWindowTitle() 
{
	if (currentFilePath == "") {
		setWindowTitle("MapEditor");
	}
	else {
		setWindowTitle(fmt::format("MapEditor - {0}", currentFilePath).c_str());
	}
}

void MainForm::refreshRecentMapsMenu() 
{
	ConfigurationManager configManager(userConfigFolder + "config.json");
	auto recents = configManager.getVectorOfStringValue(ConfigurationManager::RECENT_MAPS);
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
	//Load existing recent maps
	ConfigurationManager configManager(userConfigFolder + "config.json");
	auto recents = configManager.getVectorOfStringValue(ConfigurationManager::RECENT_MAPS);
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
	configManager.setVectorOfStringValue(ConfigurationManager::RECENT_MAPS, recents);
	configManager.save();
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
	if (style == "Dark") {
		QFile file(fmt::format("{0}/res/darkstyle/darkstyle.qss", getExecutablePath()).c_str());
		file.open(QFile::ReadOnly);
		const QString styleSheet = QLatin1String(file.readAll());
		this->setStyleSheet(styleSheet);
		ui.action_DarkTheme->setChecked(true);
	}
	else {
		this->setStyleSheet("");
		ui.action_LightTheme->setChecked(true);
	}
}

void MainForm::resizeEvent(QResizeEvent *)
{
    ui.mapOpenGLWidget->resizeGL(ui.mapOpenGLWidget->width(), ui.mapOpenGLWidget->height());
}

void MainForm::onTileSelected(MapTile *, Point) 
{
	ui.toolBox->setCurrentWidget(ui.page_TileProperties);
}

void MainForm::onTextureAdded(TextureInfo textureInfo) 
{
	if (!controller.addTexture(textureInfo)) {
		ErrorMessage::show(controller.getLastError());
	}
	refreshTextureList();
}

void MainForm::onTextureUpdated(const std::string &name, TextureInfo textureInfo) 
{
	if (!controller.replaceTexture(name, textureInfo)) {
		ErrorMessage::show(controller.getLastError());
	}
	refreshTextureList();
}

void MainForm::onTextureDeleted(const std::string &name) 
{
	if (!controller.removeTexture(name)) {
		ErrorMessage::show(controller.getLastError());
	}
	refreshTextureList();
}

void MainForm::refreshTextureList() 
{
	textureListTabComponent.refreshTextureList();
	textureSelectionComponent.refreshTextureList();
	glComponent.reloadTextures();
}

/*
void MainForm::onComboBoxTileTriggerChanged() 
{
	if (currentMapTile != nullptr) {
		currentMapTile->setTrigger(static_cast<MapTileTriggerEvent>(ui.comboBoxTileTrigger->currentIndex()));
		ui.mapOpenGLWidget->updateGL();
	}
}

void MainForm::onComboBoxTileConditionChanged() 
{
	if (currentMapTile != nullptr) {
		currentMapTile->setCondition(static_cast<MapTileTriggerCondition>(ui.comboBoxTileCondition->currentIndex()));
		ui.mapOpenGLWidget-gyugyug>updateGL();
	}
}

void MainForm::onPushButtonTileActionPropertiesClick() 
{
	if (currentMapTile != nullptr && ui.comboBoxTileAction->currentIndex() != static_cast<int>(MapTileTriggerAction::None)) {
		ui.mapOpenGLWidget->stopAutoUpdate();
		if (ui.comboBoxTileAction->currentIndex() == static_cast<int>(MapTileTriggerAction::ChangeMap)) {
			EditTileActionChangeMapPropertiesForm formEditActionProperties(this, 
																		   getResourcesPath(), 
																		   currentMapTile->getActionProperties());
			if (formEditActionProperties.exec() == QDialog::Accepted) {
				currentMapTile->setActionProperties(formEditActionProperties.getUpdatedProperties());
			}
		}
		ui.mapOpenGLWidget->startAutoUpdate();
	}
}
*/