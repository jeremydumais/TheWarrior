#include "mainForm.hpp"
#include "aboutBoxForm.hpp"
#include "configurationManager.hpp"
#include "errorMessage.hpp"
#include "manageTexturesForm.hpp"
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

	initializeItemsTableControl();
	connectUIActions();
}

void MainForm::initializeItemsTableControl()
{
	ui.tableWidgetMonsters->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
	ui.tableWidgetMonsters->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
	ui.tableWidgetMonsters->setHorizontalHeaderItem(2, new QTableWidgetItem("Health"));
	ui.tableWidgetMonsters->setHorizontalHeaderItem(3, new QTableWidgetItem("Attack"));
	ui.tableWidgetMonsters->setHorizontalHeaderItem(4, new QTableWidgetItem("Defense"));
	ui.tableWidgetMonsters->setColumnWidth(0, 120);
	ui.tableWidgetMonsters->setColumnWidth(1, 300);
	ui.tableWidgetMonsters->setColumnWidth(2, 120);
	ui.tableWidgetMonsters->setColumnWidth(3, 120);
	ui.tableWidgetMonsters->setColumnWidth(4, 120);
}

void MainForm::connectUIActions()
{
	connect(ui.action_Quit, &QAction::triggered, this, &MainForm::close);
	connect(ui.action_LightTheme, &QAction::triggered, this, &MainForm::action_LightTheme_Click);
	connect(ui.action_DarkTheme, &QAction::triggered, this, &MainForm::action_DarkTheme_Click);
	connect(ui.action_About, &QAction::triggered, this, &MainForm::action_About_Click);
	connect(ui.action_Open, &QAction::triggered, this, &MainForm::action_OpenItemStore_Click);
	connect(ui.action_RecentMap1, &QAction::triggered, this, &MainForm::action_OpenRecentItemsDB_Click);
	connect(ui.action_RecentMap2, &QAction::triggered, this, &MainForm::action_OpenRecentItemsDB_Click);
	connect(ui.action_RecentMap3, &QAction::triggered, this, &MainForm::action_OpenRecentItemsDB_Click);
	connect(ui.action_RecentMap4, &QAction::triggered, this, &MainForm::action_OpenRecentItemsDB_Click);
	connect(ui.action_RecentMap5, &QAction::triggered, this, &MainForm::action_OpenRecentItemsDB_Click);
	connect(ui.action_Save, &QAction::triggered, this, &MainForm::action_SaveItemStore_Click);
	connect(ui.action_SaveAs, &QAction::triggered, this, &MainForm::action_SaveAsItemStore_Click);
	connect(ui.action_ManageTextures, &QAction::triggered, this, &MainForm::action_ManageTextures_Click);
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

void MainForm::action_About_Click()
{
	AboutBoxForm aboutBoxForm(this);
	aboutBoxForm.exec();
}

void MainForm::action_LightTheme_Click()
{
	ConfigurationManager configManager(m_userConfigFolder + "config.json");
	if (configManager.load())
	{
		configManager.setStringValue(MainForm::THEME_PATH, "");
		setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
		if (!configManager.save())
		{
			ErrorMessage::show("An error occurred while saving the configuration file.",
							   configManager.getLastError());
		}
	}
	else
	{
		ErrorMessage::show("An error occurred while loading the configuration file.",
						   configManager.getLastError());
	}
}

void MainForm::action_DarkTheme_Click()
{
	ConfigurationManager configManager(m_userConfigFolder + "config.json");
	if (configManager.load())
	{
		configManager.setStringValue(MainForm::THEME_PATH, "Dark");
		setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
		if (!configManager.save())
		{
			ErrorMessage::show("An error occurred while saving the configuration file.",
							   configManager.getLastError());
		}
	}
	else
	{
		ErrorMessage::show("An error occurred while loading the configuration file.",
						   configManager.getLastError());
	}
}

void MainForm::action_OpenItemStore_Click()
{
	QString fullFilePath{QFileDialog::getOpenFileName(this,
													  tr("Open the item store"),
													  "",
													  tr("Item store file (*.itm)"))};
	if (fullFilePath != "")
	{
		openItemStore(fullFilePath.toStdString());
		m_currentFilePath = fullFilePath.toStdString();
		refreshItemsTable();
	}
	refreshWindowTitle();
}

void MainForm::action_OpenRecentItemsDB_Click()
{
	QAction *recentAction = qobject_cast<QAction *>(sender());
	std::string filename = recentAction->text().toStdString();
	openItemStore(filename);
}

void MainForm::action_SaveItemStore_Click()
{
	if (m_currentFilePath == "")
	{
		action_SaveAsItemStore_Click();
	}
	else
	{
		saveItemStore(m_currentFilePath);
	}
}

void MainForm::action_SaveAsItemStore_Click()
{
	QString filter = "Item store file (*.itm)";
	QString fullFilePath{QFileDialog::getSaveFileName(this,
													  tr("Save the item store"),
													  "",
													  filter, &filter)};
	if (fullFilePath != "")
	{
		m_currentFilePath = fullFilePath.toStdString();
		saveItemStore(m_currentFilePath);
		addNewRecentItemsDB(m_currentFilePath);
	}
	refreshWindowTitle();
}

void MainForm::refreshRecentMapsMenu()
{
	auto recents = std::vector<std::string>{};
	ConfigurationManager configManager(m_userConfigFolder + "config.json");
	if (configManager.load())
	{
		recents = configManager.getVectorOfStringValue(MainForm::RECENT_DB);
	}
	else
	{
		ErrorMessage::show("An error occurred while loading the configuration file.",
						   configManager.getLastError());
		return;
	}

	if (recents.size() > 5)
	{
		recents.resize(5);
	}
	ui.menu_RecentMonstersDB->setEnabled(!recents.empty());
	std::vector<QAction *> actions{
		ui.action_RecentMap1,
		ui.action_RecentMap2,
		ui.action_RecentMap3,
		ui.action_RecentMap4,
		ui.action_RecentMap5};
	for (size_t i = 0; i < recents.size(); i++)
	{
		actions[i]->setVisible(true);
		actions[i]->setText(recents[i].c_str());
	}
}

void MainForm::addNewRecentItemsDB(const std::string &filePath)
{
	auto recents = std::vector<std::string>{};
	// Load existing recent maps
	ConfigurationManager configManager(m_userConfigFolder + "config.json");
	if (configManager.load())
	{
		recents = configManager.getVectorOfStringValue(MainForm::RECENT_DB);
	}
	else
	{
		ErrorMessage::show("An error occurred while loading the configuration file.",
						   configManager.getLastError());
		return;
	}
	// Scan to find the currentMap, if found remove it from the list
	auto iter = std::find(recents.begin(), recents.end(), filePath);
	if (iter != recents.end())
	{
		recents.erase(iter);
	}
	// Add it at the beginning of the vector
	recents.insert(recents.begin(), filePath);
	if (recents.size() > 5)
	{
		recents.resize(5);
	}
	configManager.setVectorOfStringValue(MainForm::RECENT_DB, recents);
	if (!configManager.save())
	{
		ErrorMessage::show("An error occurred while saving the configuration file.",
						   configManager.getLastError());
		return;
	}
	refreshRecentMapsMenu();
}

void MainForm::refreshItemsTable()
{
	ui.tableWidgetMonsters->model()->removeRows(0, ui.tableWidgetMonsters->rowCount());
	auto monstersToDisplay = m_controller.getMonsters();
	std::vector<std::string> monsterIds;
	std::transform(monstersToDisplay.begin(),
									monstersToDisplay.end(),
									std::back_inserter(monsterIds),
									[](const MonsterListDisplay &monsterDisplay) -> std::string { return monsterDisplay.id; });
	auto monsterIdsWithIcon = m_controller.getIconsFromMonsterIds(monsterIds, getResourcesPath());
	int index = 0;
	for (const auto &monster : monstersToDisplay)
	{
		ui.tableWidgetMonsters->insertRow(index);
		auto idMonster = new QTableWidgetItem(monster.id.c_str());
		if (monsterIdsWithIcon.find(monster.id) != monsterIdsWithIcon.end()) {
			idMonster->setIcon(monsterIdsWithIcon[monster.id]);
		}
		ui.tableWidgetMonsters->setItem(index, 0, idMonster);
		ui.tableWidgetMonsters->setItem(index, 1, new QTableWidgetItem(monster.name.c_str()));
		ui.tableWidgetMonsters->setItem(index, 2, new QTableWidgetItem(monster.health));
		ui.tableWidgetMonsters->setItem(index, 3, new QTableWidgetItem(std::to_string(monster.attack).c_str()));
		ui.tableWidgetMonsters->setItem(index, 4, new QTableWidgetItem(std::to_string(monster.defense).c_str()));
		index++;
	}
}

void MainForm::action_ManageTextures_Click()
{
	ManageTexturesForm manageTexturesForm(this,
										  getResourcesPath(),
										  m_controller.getTextureContainerForEdition());
	manageTexturesForm.exec();
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
	/****
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

void MainForm::openItemStore(const std::string &filePath)
{
	if (!m_controller.openMonsterStore(filePath))
	{
		ErrorMessage::show("An error occurred while loading the monster store.",
						   m_controller.getLastError());
		return;
	}
	m_currentFilePath = filePath;
	addNewRecentItemsDB(m_currentFilePath);
	refreshItemsTable();
}

void MainForm::saveItemStore(const std::string &filePath)
{
	if (!m_controller.saveMonsterStore(filePath))
	{
		ErrorMessage::show("An error occurred while saving the monster store.",
						   m_controller.getLastError());
	}
}

void MainForm::refreshWindowTitle()
{
	setWindowTitle(m_currentFilePath.empty() ? "MonsterEditor" : fmt::format("MonsterEditor - {0}", m_currentFilePath).c_str());
}
