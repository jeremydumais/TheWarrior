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
	  map(nullptr),
	  functionAfterShownCalled(false)
{
	ui.setupUi(this);
	connectUIActions();

	ui.mapOpenGLWidget->setExecutablePath(getExecutablePath());
	map = make_shared<GameMap>(10, 10);
	TextureInfo textureInfo {
		"terrain1", "tile2.png",
		256, 704,
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
	connect(ui.mapOpenGLWidget, &MapOpenGLWidget::onTileClicked, this, &MainForm::onTileClicked);
	connect(ui.pushButtonViewTexture, &QPushButton::clicked, this, &MainForm::onPushButtonViewTextureClick);
}

MainForm::~MainForm()
{
}

void MainForm::functionAfterShown()
{
	//setWindowIcon(QIcon(":/global/TeacherHelper Icon256.png"));
}

std::string MainForm::getExecutablePath() 
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        return dirname(result);
    }
    else {
        return "";
    }
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
		ui.toolBox->setCurrentWidget(ui.page_TileProperties);
	}
}

void MainForm::refreshTextureList() 
{
	ui.listWidgetTextures->model()->removeRows(0, ui.listWidgetTextures->count());
	int index {0};
	for(const auto &texture : map->getTextures()) {
		ui.listWidgetTextures->insertItem(index, texture.getName().c_str());
		index++;
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