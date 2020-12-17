#include "mainForm.hpp"
//#include "aboutBoxForm.h"
#include <algorithm>
#include <fmt/format.h>
#include <QtCore/qfile.h>
#include <QtWidgets/qmessagebox.h>
#include <qtimer.h>
#include <QtOpenGL/QtOpenGL>
#ifdef _WIN32
	#include <windows.h>
#endif

using namespace std;

MainForm::MainForm(QWidget *parent)
	: QMainWindow(parent),
	  ui(Ui::MainForm()),
	  map(nullptr),
	  functionAfterShownCalled(false)
{
	ui.setupUi(this);
	connectUIActions();

	map = make_shared<GameMap>(10, 10);
	map->addTexture("terrain1", "tile2.png");
	ui.mapOpenGLWidget->setCurrentMap(map);
	ui.lineEditMapWidth->setText(to_string(map->getWidth()).c_str());
	ui.lineEditMapHeight->setText(to_string(map->getHeight()).c_str());
}

void MainForm::connectUIActions() 
{
	connect(ui.action_Quit, &QAction::triggered, this, &MainForm::close);
	connect(ui.action_About, &QAction::triggered, this, &MainForm::action_About_Click);
	connect(ui.action_LightTheme, &QAction::triggered, this, &MainForm::action_LightTheme_Click);
	connect(ui.action_DarkTheme, &QAction::triggered, this, &MainForm::action_DarkTheme_Click);
	connect(ui.mapOpenGLWidget, &MapOpenGLWidget::onTileClicked, this, &MainForm::onTileClicked);
}

MainForm::~MainForm()
{
}

void MainForm::functionAfterShown()
{
	//setWindowIcon(QIcon(":/global/TeacherHelper Icon256.png"));
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
	showErrorMessage(fmt::format("{}", tileIndex), "");
}