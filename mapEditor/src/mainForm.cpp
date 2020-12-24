#include "mainForm.hpp"
#include "viewTextureForm.h"
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
	  lastSelectedTextureIndex(-1),
	  functionAfterShownCalled(false), 
	  executablePath(""),
	  resourcesPath(""),
	  lastBulkUpdatedTileIndex(-1)
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
	connect(ui.pushButtonViewTexture, &QPushButton::clicked, this, &MainForm::onPushButtonViewTextureClick);
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

		//currentMapTile->setTextureIndex(lastSelectedTextureIndex);
	}
	else {
		currentMapTile = nullptr;
	}
}

void MainForm::onTileMouseReleaseEvent(int tileIndex) 
{
	if (selectionMode == SelectionMode::ApplyTexture) {
		currentMapTile = &map->getTileForEditing(tileIndex);
		currentMapTile->setTextureName(lastSelectedTextureName);
		currentMapTile->setTextureIndex(lastSelectedTextureIndex);
	}
	else if (selectionMode == SelectionMode::ApplyObject) {
		currentMapTile = &map->getTileForEditing(tileIndex);
		currentMapTile->setObjectTextureName(lastSelectedTextureName);
		currentMapTile->setObjectTextureIndex(lastSelectedTextureIndex);
	}
	lastBulkUpdatedTileIndex = -1;
	setCursor(Qt::CursorShape::ArrowCursor);
}

void MainForm::onTileMouseMoveEvent(bool mousePressed, int tileIndex) 
{

	if (mousePressed && selectionMode == SelectionMode::ApplyTexture && tileIndex != lastBulkUpdatedTileIndex) {
		setCursor(Qt::CursorShape::PointingHandCursor);
		if (currentMapTile != nullptr) {
			currentMapTile = &map->getTileForEditing(tileIndex);
			currentMapTile->setTextureName(lastSelectedTextureName);
			currentMapTile->setTextureIndex(lastSelectedTextureIndex);
			lastBulkUpdatedTileIndex = tileIndex;
		}
	}
	else if (mousePressed && selectionMode == SelectionMode::ApplyObject && tileIndex != lastBulkUpdatedTileIndex) {
		setCursor(Qt::CursorShape::PointingHandCursor);
		if (currentMapTile != nullptr) {
			currentMapTile = &map->getTileForEditing(tileIndex);
			currentMapTile->setObjectTextureName(lastSelectedTextureName);
			currentMapTile->setObjectTextureIndex(lastSelectedTextureIndex);
			lastBulkUpdatedTileIndex = tileIndex;
		}
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
}

void MainForm::onPushButtonViewTextureClick() 
{
	if (ui.listWidgetTextures->selectionModel()->hasSelection()) {
		//Find the selected texture
		auto selectedItemName { ui.listWidgetTextures->selectionModel()->selectedRows()[0].data().toString().toStdString() };
		auto selectedTexture { map->getTextureByName(selectedItemName) };
		if (selectedTexture.has_value()) {
			ViewTextureForm formViewTexture(this, getExecutablePath(), selectedTexture.get_ptr());
			formViewTexture.exec();
		}
	}
}

void MainForm::onLabelImageTextureMouseReleaseEvent(QMouseEvent *event) 
{
	int comboBoxTextureCurrentIndex { ui.comboBoxTexture->currentIndex() };
	std::string textureName { ui.comboBoxTexture->itemText(comboBoxTextureCurrentIndex).toStdString() };
	auto texture { map->getTextureByName(textureName) };
	if (texture.has_value()) {
		lastSelectedTextureName = textureName;
		lastSelectedTextureIndex = getTextureIndexFromPosition(event->pos(), texture.get());
		//showErrorMessage(fmt::format("{0}", lastTextureIndex), "");
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