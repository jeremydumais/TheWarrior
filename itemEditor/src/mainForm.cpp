#include "mainForm.hpp"
#include "aboutBoxForm.hpp"
#include "configurationManager.hpp"
#include "errorMessage.hpp"
#include "specialFolders.hpp"
#include <QtCore/qfile.h>
#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <libgen.h>         // dirname
#include <linux/limits.h>   // PATH_MAX
#include <unistd.h>         // readlink

using namespace std;

MainForm::MainForm(QWidget *parent)
	: QMainWindow(parent),
	  ui(Ui::MainForm()),
	  functionAfterShownCalled(false)
{
	ui.setupUi(this);

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

    ui.listWidgetItemCategories->setFixedWidth(300);

    connectUIActions();
}

void MainForm::connectUIActions() 
{
	connect(ui.action_Quit, &QAction::triggered, this, &MainForm::close);
    connect(ui.action_LightTheme, &QAction::triggered, this, &MainForm::action_LightTheme_Click);
	connect(ui.action_DarkTheme, &QAction::triggered, this, &MainForm::action_DarkTheme_Click);
	connect(ui.action_About, &QAction::triggered, this, &MainForm::action_About_Click);
}

void MainForm::functionAfterShown()
{
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
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

const string &MainForm::getExecutablePath()
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

const string &MainForm::getResourcesPath()
{
	if (resourcesPath.empty()) {
		resourcesPath = fmt::format("{0}/resources/", getExecutablePath());
	}
	return resourcesPath;
}

void MainForm::setAppStylesheet(const string &style)
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