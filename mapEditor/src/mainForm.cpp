#include "mainForm.hpp"
#include "configurationManager.hpp"
#include "editTextureForm.hpp"
#include "editMapTileTriggerForm.hpp"
#include "editTileActionChangeMapPropertiesForm.hpp"
#include "aboutBoxForm.hpp"
#include "specialFolders.hpp"
#include <algorithm>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <fstream>
#include <QtCore/qfile.h>
#include <QtWidgets/qmessagebox.h>
#include <qtimer.h>
#include <QtOpenGL/QtOpenGL>
#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX
using namespace std;

MainForm::MainForm(QWidget *parent)
	: QMainWindow(parent),
	  ui(Ui::MainForm()),
	  selectionMode(SelectionMode::Select),
	  currentFilePath(""),
	  functionAfterShownCalled(false), 
	  executablePath(""),
	  resourcesPath("")
{
	ui.setupUi(this);
	glComponent.initializeUIObjects(ui.mapOpenGLWidget);
	MainForm_TileTabComponent_Objects tileUIObjects;
	tileUIObjects.labelTileCoordXY = ui.labelTileCoordXY;
	tileUIObjects.lineEditTexName = ui.lineEditTexName;
	tileUIObjects.spinBoxTexIndex = ui.spinBoxTexIndex;
	tileUIObjects.lineEditObjTexName = ui.lineEditObjTexName;
	tileUIObjects.spinBoxObjTexIndex = ui.spinBoxObjTexIndex;
	tileUIObjects.checkBoxObjectAbovePlayer = ui.checkBoxObjectAbovePlayer;
	tileUIObjects.checkBoxTileCanSteppedOn = ui.checkBoxTileCanSteppedOn;
	tileTabComponent.initializeUIObjects(tileUIObjects);
	connectUIActions();
	generateComboxItems();

	//Check if the user configuration folder exist
	userConfigFolder = SpecialFolders::getUserConfigDirectory();
	if (!boost::filesystem::exists(userConfigFolder)) {
		if (!boost::filesystem::create_directory(userConfigFolder)) {
			showErrorMessage(fmt::format("Unable to create the folder {0}", userConfigFolder), "");
			exit(1);
		}
	}

	//Check if the configuration file exist
	ConfigurationManager configManager(userConfigFolder + "config.json");
	setAppStylesheet(configManager.getStringValue(ConfigurationManager::THEME_PATH));

	ui.mapOpenGLWidget->setResourcesPath(getResourcesPath());
	//Generate a test map
	if (!controller.createMap(20, 20)) {
		showErrorMessage(controller.getLastError());
		exit(1);
	}
	if (!controller.addTexture({
		"Terrain1", "tile.png",
		256, 4256,
		32, 32
	})) {
		showErrorMessage(controller.getLastError());
		exit(1);
	}
	if (!controller.addTexture({
		"NPC1", "tileNPC1.png",
		384, 256,
		32, 32
	})) {
		showErrorMessage(controller.getLastError());
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
	connect(ui.pushButtonApplySizeChange, &QPushButton::clicked, this, &MainForm::onPushButtonApplySizeChangeClick);
	connect(ui.pushButtonAddTexture, &QPushButton::clicked, this, &MainForm::onPushButtonAddTextureClick);
	connect(ui.pushButtonEditTexture, &QPushButton::clicked, this, &MainForm::onPushButtonEditTextureClick);
	connect(ui.pushButtonDeleteTexture, &QPushButton::clicked, this, &MainForm::onPushButtonDeleteTextureClick);
	connect(ui.pushButtonSelectedTextureClear, &QPushButton::clicked, this, &MainForm::onPushButtonSelectedTextureClearClick);
	connect(ui.pushButtonSelectedObjectClear, &QPushButton::clicked, this, &MainForm::onPushButtonSelectedObjectClearClick);
	connect(ui.labelImageTexture, &QClickableLabel::onMouseReleaseEvent, this, &MainForm::onLabelImageTextureMouseReleaseEvent);
	connect(ui.lineEditTexName, &QLineEdit::textChanged, this, &MainForm::onLineEditTexNameTextChanged);
	connect(ui.spinBoxTexIndex, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainForm::onSpinBoxTexIndexValueChanged);
	connect(ui.lineEditObjTexName, &QLineEdit::textChanged, this, &MainForm::onLineEditObjTexNameTextChanged);
	connect(ui.spinBoxObjTexIndex, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainForm::onSpinBoxObjTexIndexValueChanged);
	connect(ui.checkBoxTileCanSteppedOn, &QCheckBox::stateChanged, this, &MainForm::onCheckBoxTileCanSteppedOnChanged);
	connect(ui.checkBoxObjectAbovePlayer, &QCheckBox::stateChanged, this, &MainForm::onCheckBoxObjectAbovePlayerChanged);
	/*connect(ui.comboBoxTileTrigger, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainForm::onComboBoxTileTriggerChanged);
	connect(ui.comboBoxTileCondition, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainForm::onComboBoxTileConditionChanged);
	connect(ui.comboBoxTileAction, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainForm::onComboBoxTileActionChanged);
	connect(ui.pushButtonTileActionProperties, &QPushButton::clicked, this, &MainForm::onPushButtonTileActionPropertiesClick);*/
	connect(ui.comboBoxTexture, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainForm::onComboBoxTextureCurrentIndexChanged);
	glComponent.connectUIActions();
	//Connect the glComponent and the different components
	connect(&glComponent, &MainForm_GLComponent::tileSelected, this, &MainForm::onTileSelected);
	connect(&glComponent, &MainForm_GLComponent::tileSelected, &tileTabComponent, &MainForm_TileTabComponent::onTileSelected);
	
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
		showErrorMessage("An error occurred while saving the configuration file.", 
						 configManager.getLastError());
	}
}

void MainForm::action_DarkTheme_Click()
{
	ConfigurationManager configManager(userConfigFolder + "config.json");
	configManager.setStringValue(ConfigurationManager::THEME_PATH, "Dark");
	setAppStylesheet(configManager.getStringValue(ConfigurationManager::THEME_PATH));
	if (!configManager.save()) {
		showErrorMessage("An error occurred while saving the configuration file.", 
						 configManager.getLastError());
	}
}

void MainForm::action_DisplayGrid_Click() 
{
	ui.mapOpenGLWidget->setGridEnabled(ui.action_DisplayGrid->isChecked());
}

void MainForm::action_SelectClick() 
{
	selectionMode = SelectionMode::Select;
	glComponent.setSelectionMode(selectionMode);
}

void MainForm::action_MoveMapClick() 
{
	selectionMode = SelectionMode::MoveMap;
	glComponent.setSelectionMode(selectionMode);
}

void MainForm::action_ApplyTextureClick() 
{
	selectionMode = SelectionMode::ApplyTexture;
	glComponent.setSelectionMode(selectionMode);
}

void MainForm::action_ApplyObjectClick() 
{
	selectionMode = SelectionMode::ApplyObject;
	glComponent.setSelectionMode(selectionMode);
}

void MainForm::action_EnableCanStepClick() 
{
	selectionMode = SelectionMode::EnableCanStep;
	glComponent.setSelectionMode(selectionMode);
}

void MainForm::action_DisableCanStepClick() 
{
	selectionMode = SelectionMode::DisableCanStep;
	glComponent.setSelectionMode(selectionMode);
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
		showErrorMessage(fmt::format("Unable to open the map {0}", filePath));
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

void MainForm::showErrorMessage(const string &message,
								const string &internalError) const
{
	QMessageBox msgBox;
	msgBox.setText(message.c_str());
	if (internalError.length() > 0) {
		msgBox.setInformativeText(internalError.c_str());
	}
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setWindowTitle("Error");
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();
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

void MainForm::onPushButtonApplySizeChangeClick() 
{
	int offsetLeft { ui.spinBoxMapSizeLeft->value() };
	int offsetTop { ui.spinBoxMapSizeTop->value() };
	int offsetRight { ui.spinBoxMapSizeRight->value() };
	int offsetBottom { ui.spinBoxMapSizeBottom->value() };
	if (offsetLeft < 0 || 
		offsetTop < 0 ||
		offsetRight < 0 ||
		offsetBottom < 0) {	
		//Check if there's tiles that are already assigned in the ones we will remove
		if (controller.isShrinkMapImpactAssignedTiles(offsetLeft,
													  offsetTop,
													  offsetRight,
													  offsetBottom)) {
			showErrorMessage("Cannot resize the map because some tile are\n"
								"already assigned in the ones you try to remove.");
			return;
		}
	}
	//Apply new size
	try {
		controller.resizeMap(offsetLeft,
							 offsetTop,
							 offsetRight,
							 offsetBottom);
	}
	catch(invalid_argument &err) {
		showErrorMessage(err.what());
	}
}

/*void MainForm::onTileMouseMoveEvent(bool mousePressed, int tileIndex) 
{	
}*/

void MainForm::onPushButtonAddTextureClick() 
{
	ui.mapOpenGLWidget->stopAutoUpdate();
	auto alreadyUsedTextureNames { controller.getAlreadyUsedTextureNames() };
	EditTextureForm formEditTexture(this, getResourcesPath(), nullptr, alreadyUsedTextureNames);
	if (formEditTexture.exec() == QDialog::Accepted) {
		if (!controller.addTexture(formEditTexture.getTextureInfo())) {
			showErrorMessage(controller.getLastError());
		}
		refreshTextureList();
	}
	ui.mapOpenGLWidget->startAutoUpdate();
}

void MainForm::onPushButtonEditTextureClick() 
{
	ui.mapOpenGLWidget->stopAutoUpdate();
	auto selectedTexture = getSelectedTextureInTextureList();
	if (selectedTexture.has_value()) {
		auto alreadyUsedTextureNames = controller.getAlreadyUsedTextureNames();
		//Remove the actual selected texture name
		auto iter = std::find(alreadyUsedTextureNames.begin(), alreadyUsedTextureNames.end(), selectedTexture->getName());
		if (iter != alreadyUsedTextureNames.end()) {
			alreadyUsedTextureNames.erase(iter);
		}
		EditTextureForm formEditTexture(this, getResourcesPath(), selectedTexture.get_ptr(), alreadyUsedTextureNames);
		if (formEditTexture.exec() == QDialog::Accepted) {
			const TextureInfo &updatedTextureInfo { formEditTexture.getTextureInfo() };
			if (!controller.replaceTexture(selectedTexture->getName(), updatedTextureInfo)) {
				showErrorMessage(controller.getLastError());
			}
			refreshTextureList();
		}
	}
	ui.mapOpenGLWidget->startAutoUpdate();
}

void MainForm::onPushButtonDeleteTextureClick() 
{
	auto selectedTexture = getSelectedTextureInTextureList();
	if (selectedTexture.has_value()) {
		QMessageBox msgBox;
		msgBox.setText(fmt::format("Are you sure you want to delete the texture {0}?", selectedTexture->getName()).c_str());
		msgBox.setWindowTitle("Confirmation");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if (msgBox.exec() == QMessageBox::Yes) {
			//Check if the texture is used in the map
			msgBox.setText(fmt::format("The texture {0} is used by some map tiles.\nAre you sure you want to proceed?", selectedTexture->getName()).c_str());
			bool isUsed = controller.isTextureUsedInMap(selectedTexture->getName());
			if (!isUsed || (isUsed && msgBox.exec() == QMessageBox::Yes)) {
				if (!controller.removeTexture(selectedTexture->getName())) {
					showErrorMessage(controller.getLastError());
				}
				refreshTextureList();
			}
		}
	}
}

boost::optional<const Texture &> MainForm::getSelectedTextureInTextureList() 
{
	if (ui.listWidgetTextures->selectionModel()->hasSelection()) {
		//Find the selected texture
		auto selectedItemName { ui.listWidgetTextures->selectionModel()->selectedRows()[0].data().toString().toStdString() };
		return controller.getMap()->getTextureByName(selectedItemName);
	}
	else {
		return {};
	}
}

void MainForm::refreshTextureList() 
{
	auto map { controller.getMap() };
	ui.listWidgetTextures->model()->removeRows(0, ui.listWidgetTextures->count());
	ui.comboBoxTexture->model()->removeRows(0, ui.comboBoxTexture->count());
	int index {0};
	for(const auto &texture : map->getTextures()) {
		ui.listWidgetTextures->insertItem(index, texture.getName().c_str());
		ui.comboBoxTexture->insertItem(index, texture.getName().c_str());
		index++;
	}
	displaySelectedTextureImage();
	ui.mapOpenGLWidget->reloadTextures();
}

void MainForm::displaySelectedTextureImage() 
{
	auto map { controller.getMap() };
	//Find the selected texture
	auto texture { map->getTextureByName(ui.comboBoxTexture->itemText(ui.comboBoxTexture->currentIndex()).toStdString()) };
	if (texture.has_value()) {
		QImageReader reader(fmt::format("{0}/resources/{1}", getExecutablePath(), texture->getFilename()).c_str());
		const QImage image = reader.read();
		ui.labelImageTexture->setFixedSize(image.width(), image.height());
		ui.labelImageTexture->setPixmap(QPixmap::fromImage(image));
	}
	else {
		ui.labelImageTexture->clear();
		glComponent.clearLastSelectedTexture();
		glComponent.clearLastSelectedObject();
		ui.labelSelectedTexture->clear();
		ui.labelSelectedObject->clear();
	}
}

void MainForm::onPushButtonSelectedTextureClearClick() 
{
	glComponent.clearLastSelectedTexture();
	ui.labelSelectedTexture->clear();
}

void MainForm::onPushButtonSelectedObjectClearClick() 
{
	glComponent.clearLastSelectedObject();
	ui.labelSelectedObject->clear();
}

void MainForm::onLabelImageTextureMouseReleaseEvent(QMouseEvent *event) 
{
	auto map { controller.getMap() };
	int comboBoxTextureCurrentIndex { ui.comboBoxTexture->currentIndex() };
	std::string textureName { ui.comboBoxTexture->itemText(comboBoxTextureCurrentIndex).toStdString() };
	auto texture { map->getTextureByName(textureName) };
	if (texture.has_value()) {
		string name { textureName };
		int index = controller.getTextureIndexFromPosition(to_Point(event->pos()), texture.get());
		//Display the selected texture or object on the selected image
		auto imagePart { getTextureTileImageFromTexture(index, texture.get()) };
		if (selectionMode == SelectionMode::ApplyTexture) {
			glComponent.setLastSelectedTexture(name, index);
			ui.labelSelectedTexture->setPixmap(imagePart);
		}
		else if (selectionMode == SelectionMode::ApplyObject) {
			glComponent.setLastSelectedObject(name, index);
			ui.labelSelectedObject->setPixmap(imagePart);
		}
	}	
}

void MainForm::onLineEditTexNameTextChanged(const QString &text) 
{
	auto currentMapTile = glComponent.getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setTextureName(text.toStdString());
		ui.mapOpenGLWidget->updateGL();
	}
}

void MainForm::onSpinBoxTexIndexValueChanged(int value) 
{
	auto currentMapTile = glComponent.getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setTextureIndex(value);
		ui.mapOpenGLWidget->updateGL();
	}
}

void MainForm::onLineEditObjTexNameTextChanged(const QString &text) 
{
	auto currentMapTile = glComponent.getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setObjectTextureName(text.toStdString());
		ui.mapOpenGLWidget->updateGL();
	}
}

void MainForm::onSpinBoxObjTexIndexValueChanged(int value) 
{
	auto currentMapTile = glComponent.getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setObjectTextureIndex(value);
		ui.mapOpenGLWidget->updateGL();
	}
}

void MainForm::onCheckBoxObjectAbovePlayerChanged(int state) 
{
	auto currentMapTile = glComponent.getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setObjectAbovePlayer(state == Qt::Checked);
		ui.mapOpenGLWidget->updateGL();
	}
}

void MainForm::onCheckBoxTileCanSteppedOnChanged(int state) 
{
	auto currentMapTile = glComponent.getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setCanPlayerSteppedOn(state == Qt::Checked);
		ui.mapOpenGLWidget->updateGL();
	}
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
		ui.mapOpenGLWidget->updateGL();
	}
}

void MainForm::onComboBoxTileActionChanged() 
{
	if (currentMapTile != nullptr) {
		currentMapTile->setAction(static_cast<MapTileTriggerAction>(ui.comboBoxTileAction->currentIndex()));
		ui.mapOpenGLWidget->updateGL();
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
void MainForm::onComboBoxTextureCurrentIndexChanged() 
{
	displaySelectedTextureImage();
}

QPixmap MainForm::getTextureTileImageFromTexture(int tileIndex, const Texture &texture) const 
{
	int textureWidthInPixel { texture.getWidth() };
	int textureHeightInPixel { texture.getHeight() };
	int x { (tileIndex * texture.getTileWidth()) % textureWidthInPixel };
	int y { textureHeightInPixel - (((tileIndex * texture.getTileWidth()) / textureWidthInPixel) * texture.getTileHeight()) };
	QPixmap imagePart = ui.labelImageTexture->pixmap()->copy(x, y - texture.getTileHeight(), texture.getTileWidth(), texture.getTileHeight());
	return imagePart;
}

Point MainForm::to_Point(QPoint point) 
{
	return Point(point.x(), point.y());
}