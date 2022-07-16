#include "mainForm.hpp"
#include "configurationManager.hpp"
#include "errorMessage.hpp"
#include "specialFolders.hpp"
#include <fmt/format.h>
#include <QtCore/qfile.h>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <boost/filesystem.hpp>
#include <libgen.h>		  // dirname
#include <linux/limits.h> // PATH_MAX
#include <string>
#include <unistd.h> // readlink

const std::string MainForm::THEME_PATH{"Display.Theme"};
const std::string MainForm::RECENT_DB{"MonsterDB.Recents"};

MainForm::MainForm(QWidget *parent)
	: QMainWindow(parent),
	  ui(Ui::MainForm()),
	  m_functionAfterShownCalled(false),
	  m_userConfigFolder(""),
	  m_executablePath(""),
	  m_resourcesPath(""),
	  m_currentFilePath(""),
	  m_controller(MainController())
{
	ui.setupUi(this);

	// Check if the user configuration folder exist
	m_userConfigFolder = SpecialFolders::getAppConfigDirectory("TheWarrior_MonsterEditor");
	if (!boost::filesystem::exists(m_userConfigFolder))
	{
		if (!boost::filesystem::create_directory(m_userConfigFolder))
		{
			ErrorMessage::show(fmt::format("Unable to create the folder {0}", m_userConfigFolder), "");
			exit(1);
		}
	}
	// Check if the configuration file exist
	ConfigurationManager configManager(m_userConfigFolder + "config.json");
	if (!configManager.fileExists())
	{
		// Try to create a default configuration
		if (!configManager.save())
		{
			ErrorMessage::show("An error occurred while creation a default the configuration file.",
							   configManager.getLastError());
		}
	}
	// Load the config file
	if (configManager.load())
	{
		setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
	}
	else
	{
		ErrorMessage::show("An error occurred while loading the configuration file.",
						   configManager.getLastError());
	}
}

void MainForm::connectUIActions()
{
}

void MainForm::functionAfterShown()
{
	setWindowIcon(QIcon(":/MonsterEditor Icon.png"));
}

bool MainForm::event(QEvent *event)
{
	const bool ret_val = QMainWindow::event(event);
	if (!m_functionAfterShownCalled && event->type() == QEvent::Paint)
	{
		m_functionAfterShownCalled = true;
		functionAfterShown();
	}
	return ret_val;
}

const std::string &MainForm::getExecutablePath()
{
	if (m_executablePath.empty())
	{
		char result[PATH_MAX] = {};
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		if (count != -1)
		{
			m_executablePath = dirname(result);
		}
	}
	return m_executablePath;
}

const std::string &MainForm::getResourcesPath()
{
	if (m_resourcesPath.empty())
	{
		m_resourcesPath = fmt::format("{0}/resources/", getExecutablePath());
	}
	return m_resourcesPath;
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
	if (style == "Dark")
	{
		QFile file(fmt::format("{0}/darkstyle/darkstyle.qss", getResourcesPath()).c_str());
		file.open(QFile::ReadOnly);
		const QString styleSheet = QLatin1String(file.readAll());
		this->setStyleSheet(styleSheet);
		ui.action_DarkTheme->setChecked(true);
	}
	else
	{
		this->setStyleSheet("");
		ui.action_LightTheme->setChecked(true);
	}
}
