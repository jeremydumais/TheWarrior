#include "mainForm.hpp"
#include "editTextureForm.hpp"
#include <algorithm>
#include <fmt/format.h>
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
	  map(nullptr),
	  currentMapTile(nullptr),
	  lastSelectedTextureName(""),
	  lastSelectedObjectName(""),
	  lastSelectedTextureIndex(-1),
	  lastSelectedObjectIndex(-1),
	  functionAfterShownCalled(false), 
	  executablePath(""),
	  resourcesPath("")
{
	ui.setupUi(this);
	connectUIActions();

	ui.mapOpenGLWidget->setResourcesPath(getResourcesPath());
	map = make_shared<GameMap>(20, 20);
	TextureInfo textureInfo {
		"terrain1", "tile.png",
		256, 4256,
		32, 32
	};
	map->addTexture(textureInfo);
	ui.mapOpenGLWidget->setCurrentMap(map);
	ui.lineEditMapWidth->setText(to_string(map->getWidth()).c_str());
	ui.lineEditMapHeight->setText(to_string(map->getHeight()).c_str());
	refreshTextureList();
}

void MainForm::connectUIActions() 
{
	connect(ui.action_Quit, &QAction::triggered, this, &MainForm::close);
	connect(ui.action_About, &QAction::triggered, this, &MainForm::action_About_Click);
	connect(ui.action_LightTheme, &QAction::triggered, this, &MainForm::action_LightTheme_Click);
	connect(ui.action_DarkTheme, &QAction::triggered, this, &MainForm::action_DarkTheme_Click);
	connect(ui.action_Select, &QAction::triggered, this, &MainForm::action_SelectClick);
	connect(ui.action_MoveMap, &QAction::triggered, this, &MainForm::action_MoveMapClick);
	connect(ui.action_ApplyTexture, &QAction::triggered, this, &MainForm::action_ApplyTextureClick);
	connect(ui.action_ApplyObject, &QAction::triggered, this, &MainForm::action_ApplyObjectClick);
	connect(ui.mapOpenGLWidget, &MapOpenGLWidget::onTileClicked, this, &MainForm::onTileClicked);
	connect(ui.mapOpenGLWidget, &MapOpenGLWidget::onTileMouseReleaseEvent, this, &MainForm::onTileMouseReleaseEvent);
	connect(ui.mapOpenGLWidget, &MapOpenGLWidget::onTileMouseMoveEvent, this, &MainForm::onTileMouseMoveEvent);
	connect(ui.pushButtonAddTexture, &QPushButton::clicked, this, &MainForm::onPushButtonAddTextureClick);
	connect(ui.pushButtonEditTexture, &QPushButton::clicked, this, &MainForm::onPushButtonEditTextureClick);
	connect(ui.pushButtonDeleteTexture, &QPushButton::clicked, this, &MainForm::onPushButtonDeleteTextureClick);
	connect(ui.pushButtonSelectedTextureClear, &QPushButton::clicked, this, &MainForm::onPushButtonSelectedTextureClearClick);
	connect(ui.pushButtonSelectedObjectClear, &QPushButton::clicked, this, &MainForm::onPushButtonSelectedObjectClearClick);
	connect(ui.labelImageTexture, &QClickableLabel::onMouseReleaseEvent, this, &MainForm::onLabelImageTextureMouseReleaseEvent);
	connect(ui.lineEditTexIndex, &QLineEdit::textChanged, this, &MainForm::onLineEditTexIndexTextChange);
}

MainForm::~MainForm()
{
}

void MainForm::functionAfterShown()
{
	//setWindowIcon(QIcon(":/global/TeacherHelper Icon256.png"));
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
	/*AboutBoxForm aboutBoxForm(this);
	aboutBoxForm.exec();*/
}

void MainForm::action_LightTheme_Click()
{
	/*ConfigurationManager configManager(userConfigFolder + "config.json");
	configManager.setStringValue(ConfigurationManager::THEME_PATH, "");
	setAppStylesheet(configManager.getStringValue(ConfigurationManager::THEME_PATH));
	if (!configManager.save()) {
		showErrorMessage("An error occurred while saving the configuration file.", 
						 configManager.getLastError());
	}*/
}

void MainForm::action_DarkTheme_Click()
{
	/*ConfigurationManager configManager(userConfigFolder + "config.json");
	configManager.setStringValue(ConfigurationManager::THEME_PATH, "Dark");
	setAppStylesheet(configManager.getStringValue(ConfigurationManager::THEME_PATH));
	if (!configManager.save()) {
		showErrorMessage("An error occurred while saving the configuration file.", 
						 configManager.getLastError());
	}*/
}

void MainForm::action_SelectClick() 
{
	selectionMode = SelectionMode::Select;
	ui.mapOpenGLWidget->setSelectionMode(selectionMode);
}

void MainForm::action_MoveMapClick() 
{
	selectionMode = SelectionMode::MoveMap;
	ui.mapOpenGLWidget->setSelectionMode(selectionMode);
}

void MainForm::action_ApplyTextureClick() 
{
	selectionMode = SelectionMode::ApplyTexture;
	ui.mapOpenGLWidget->setSelectionMode(selectionMode);
}

void MainForm::action_ApplyObjectClick() 
{
	selectionMode = SelectionMode::ApplyObject;
	ui.mapOpenGLWidget->setSelectionMode(selectionMode);
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
		#ifdef _WIN32
			char exePath[MAX_PATH]; 
			// When NULL is passed to GetModuleHandle, the handle of the exe itself is returned
			HMODULE hModule = GetModuleHandle(nullptr);
			if (hModule != nullptr)
			{
				// Use GetModuleFileName() with module handle to get the path
				GetModuleFileName(hModule, exePath, (sizeof(exePath))); 
			}
			boost::filesystem::path path(exePath);
			string stylePath { fmt::format("{0}\\res\\", path.parent_path().string())};
		#else
			string stylePath = "/usr/local/share/TeacherHelperApp/res/";
		#endif
		QFile file(fmt::format("{0}darkstyle.qss", stylePath).c_str());
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

void MainForm::onTileClicked(int tileIndex) 
{
	if (tileIndex != -1) {
		currentMapTile = &map->getTileForEditing(tileIndex);
		ui.lineEditTexName->setText(currentMapTile->getTextureName().c_str());
		ui.lineEditTexIndex->setText(to_string(currentMapTile->getTextureIndex()).c_str());
		ui.lineEditObjTexName->setText(currentMapTile->getObjectTextureName().c_str());
		ui.lineEditObjTexIndex->setText(to_string(currentMapTile->getObjectTextureIndex()).c_str());
		ui.toolBox->setCurrentWidget(ui.page_TileProperties);
	}
	else {
		currentMapTile = nullptr;
	}
}

void MainForm::onTileMouseReleaseEvent(vector<int> selectedTileIndexes) 
{
	if (selectionMode == SelectionMode::ApplyTexture) {
		for(const int index : selectedTileIndexes) {
			currentMapTile = &map->getTileForEditing(index);
			currentMapTile->setTextureName(lastSelectedTextureName);
			currentMapTile->setTextureIndex(lastSelectedTextureIndex);
		}
	}
	else if (selectionMode == SelectionMode::ApplyObject) {
		for(const int index : selectedTileIndexes) {
			currentMapTile = &map->getTileForEditing(index);
			currentMapTile->setObjectTextureName(lastSelectedObjectName);
			currentMapTile->setObjectTextureIndex(lastSelectedObjectIndex);
		}
	}
	setCursor(Qt::CursorShape::ArrowCursor);
}

void MainForm::onTileMouseMoveEvent(bool mousePressed, int tileIndex) 
{
}

void MainForm::onPushButtonAddTextureClick() 
{
	auto alreadyUsedTextureNames { getAlreadyUsedTextureNames() };
	EditTextureForm formEditTexture(this, getResourcesPath(), nullptr, alreadyUsedTextureNames);
	if (formEditTexture.exec() == QDialog::Accepted) {
		map->addTexture(formEditTexture.getTextureInfo());
		refreshTextureList();
	}
}

void MainForm::onPushButtonEditTextureClick() 
{
	auto selectedTexture = getSelectedTextureInTextureList();
	if (selectedTexture.has_value()) {
		auto alreadyUsedTextureNames = getAlreadyUsedTextureNames();
		//Remove the actual selected texture name
		auto iter = std::find(alreadyUsedTextureNames.begin(), alreadyUsedTextureNames.end(), selectedTexture->getName());
		if (iter != alreadyUsedTextureNames.end()) {
			alreadyUsedTextureNames.erase(iter);
		}
		EditTextureForm formEditTexture(this, getResourcesPath(), selectedTexture.get_ptr(), alreadyUsedTextureNames);
		if (formEditTexture.exec() == QDialog::Accepted) {
			if (!map->replaceTexture(selectedTexture->getName(), formEditTexture.getTextureInfo())) {
				showErrorMessage(map->getLastError(), "");
			}
			refreshTextureList();
		}
	}
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
			if (!map->removeTexture(selectedTexture->getName())) {
				showErrorMessage(map->getLastError(), "");
			}
			refreshTextureList();
		}

	}
}

std::vector<std::string> MainForm::getAlreadyUsedTextureNames() const
{
	vector<string> alreadyUsedTextureNames;
    transform(map->getTextures().begin(), map->getTextures().end(), std::back_inserter(alreadyUsedTextureNames),
               [](Texture const& x) { return x.getName(); });
	return alreadyUsedTextureNames;
}

boost::optional<const Texture &> MainForm::getSelectedTextureInTextureList() 
{
	if (ui.listWidgetTextures->selectionModel()->hasSelection()) {
		//Find the selected texture
		auto selectedItemName { ui.listWidgetTextures->selectionModel()->selectedRows()[0].data().toString().toStdString() };
		return map->getTextureByName(selectedItemName);
	}
	else {
		return {};
	}
}

void MainForm::refreshTextureList() 
{
	ui.listWidgetTextures->model()->removeRows(0, ui.listWidgetTextures->count());
	ui.comboBoxTexture->model()->removeRows(0, ui.comboBoxTexture->count());
	int index {0};
	for(const auto &texture : map->getTextures()) {
		ui.listWidgetTextures->insertItem(index, texture.getName().c_str());
		ui.comboBoxTexture->insertItem(index, texture.getName().c_str());
		index++;
	}
	//Find the selected texture
	auto texture { map->getTextureByName(ui.comboBoxTexture->itemText(ui.comboBoxTexture->currentIndex()).toStdString()) };
	if (texture.has_value()) {
		QImageReader reader(fmt::format("{0}/resources/{1}", getExecutablePath(), texture->getFilename()).c_str());
		const QImage image = reader.read();
		ui.labelImageTexture->setFixedSize(image.width(), image.height());
		//this->setFixedSize(image.width() + 20, image.height() + 80);
		ui.labelImageTexture->setPixmap(QPixmap::fromImage(image));
	}
	else {
		ui.labelImageTexture->clear();
		lastSelectedTextureName = "";
		lastSelectedTextureIndex = -1;
		lastSelectedObjectName = "";
		lastSelectedObjectIndex = -1;
		ui.labelSelectedTexture->clear();
		ui.labelSelectedObject->clear();
	}
	ui.mapOpenGLWidget->reloadTextures();
}

void MainForm::onPushButtonSelectedTextureClearClick() 
{
	lastSelectedTextureName = "";
	lastSelectedTextureIndex = -1;
	ui.labelSelectedTexture->clear();
}

void MainForm::onPushButtonSelectedObjectClearClick() 
{
	lastSelectedObjectName = "";
	lastSelectedObjectIndex = -1;
	ui.labelSelectedObject->clear();
}

void MainForm::onLabelImageTextureMouseReleaseEvent(QMouseEvent *event) 
{
	int comboBoxTextureCurrentIndex { ui.comboBoxTexture->currentIndex() };
	std::string textureName { ui.comboBoxTexture->itemText(comboBoxTextureCurrentIndex).toStdString() };
	auto texture { map->getTextureByName(textureName) };
	if (texture.has_value()) {
		string name = textureName;
		int index = getTextureIndexFromPosition(event->pos(), texture.get());
		//Display the selected texture or object on the selected image
		auto imagePart { getTextureTileImageFromTexture(index, texture.get()) };
		if (selectionMode == SelectionMode::ApplyTexture) {
			lastSelectedTextureName = name;
			lastSelectedTextureIndex = index;
			ui.labelSelectedTexture->setPixmap(imagePart);
		}
		else if (selectionMode == SelectionMode::ApplyObject) {
			lastSelectedObjectName = name;
			lastSelectedObjectIndex = index;
			ui.labelSelectedObject->setPixmap(imagePart);
		}
	}	
}

int MainForm::getTextureIndexFromPosition(const QPoint &pos, const Texture &texture) 
{
	int realY { texture.getHeight() - pos.y()};
	int indexX = pos.x() / texture.getTileWidth();
	int indexY = realY / texture.getTileHeight();
	int tileIndex { indexX + (indexY * (texture.getWidth()/texture.getTileWidth())) };
	return tileIndex;
}

void MainForm::onLineEditTexIndexTextChange(const QString &text) 
{
	if (currentMapTile != nullptr) {
		currentMapTile->setTextureIndex(text.toInt());
		ui.mapOpenGLWidget->updateGL();
	}
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