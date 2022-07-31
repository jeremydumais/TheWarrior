#include "mainForm.hpp"
#include "aboutBoxForm.hpp"
#include "configurationManager.hpp"
#include "editMonsterForm.hpp"
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
#include <optional>
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
	if (!boost::filesystem::exists(m_userConfigFolder)) {
		if (!boost::filesystem::create_directory(m_userConfigFolder)) {
			ErrorMessage::show(fmt::format("Unable to create the folder {0}", m_userConfigFolder), "");
			exit(1);
		}
	}
	// Check if the configuration file exist
	ConfigurationManager configManager(m_userConfigFolder + "config.json");
	if (!configManager.fileExists()) {
		// Try to create a default configuration
		if (!configManager.save()) {
			ErrorMessage::show("An error occurred while creation a default the configuration file.",
							   configManager.getLastError());
		}
	}
	// Load the config file
	if (configManager.load()) {
		setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
	}
	else {
		ErrorMessage::show("An error occurred while loading the configuration file.",
						   configManager.getLastError());
	}

	initializeMonstersTableControl();
    refreshRecentMapsMenu();
	connectUIActions();
    //TODO to remove (test only)
    openMonsterStore("/home/jed/Programming/TheWarrior/resources/monsters/monsterstore.mon");

}

void MainForm::initializeMonstersTableControl()
{
	ui.tableWidgetMonsters->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
	ui.tableWidgetMonsters->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
	ui.tableWidgetMonsters->setHorizontalHeaderItem(2, new QTableWidgetItem("Health"));
	ui.tableWidgetMonsters->setHorizontalHeaderItem(3, new QTableWidgetItem("Attack"));
	ui.tableWidgetMonsters->setHorizontalHeaderItem(4, new QTableWidgetItem("Defense"));
	ui.tableWidgetMonsters->setHorizontalHeaderItem(5, new QTableWidgetItem("Gold reward"));
	ui.tableWidgetMonsters->setColumnWidth(0, 120);
	ui.tableWidgetMonsters->setColumnWidth(1, 300);
	ui.tableWidgetMonsters->setColumnWidth(2, 120);
	ui.tableWidgetMonsters->setColumnWidth(3, 120);
	ui.tableWidgetMonsters->setColumnWidth(4, 120);
	ui.tableWidgetMonsters->setColumnWidth(5, 120);
}

void MainForm::connectUIActions()
{
	connect(ui.action_Quit, &QAction::triggered, this, &MainForm::close);
	connect(ui.action_LightTheme, &QAction::triggered, this, &MainForm::action_LightTheme_Click);
	connect(ui.action_DarkTheme, &QAction::triggered, this, &MainForm::action_DarkTheme_Click);
	connect(ui.action_About, &QAction::triggered, this, &MainForm::action_About_Click);
	connect(ui.action_Open, &QAction::triggered, this, &MainForm::action_OpenMonsterStore_Click);
	connect(ui.action_RecentMap1, &QAction::triggered, this, &MainForm::action_OpenRecentMonstersDB_Click);
	connect(ui.action_RecentMap2, &QAction::triggered, this, &MainForm::action_OpenRecentMonstersDB_Click);
	connect(ui.action_RecentMap3, &QAction::triggered, this, &MainForm::action_OpenRecentMonstersDB_Click);
	connect(ui.action_RecentMap4, &QAction::triggered, this, &MainForm::action_OpenRecentMonstersDB_Click);
	connect(ui.action_RecentMap5, &QAction::triggered, this, &MainForm::action_OpenRecentMonstersDB_Click);
	connect(ui.action_Save, &QAction::triggered, this, &MainForm::action_SaveMonsterStore_Click);
	connect(ui.action_SaveAs, &QAction::triggered, this, &MainForm::action_SaveAsMonsterStore_Click);
	connect(ui.action_ManageTextures, &QAction::triggered, this, &MainForm::action_ManageTextures_Click);
	connect(ui.action_AddMonster, &QAction::triggered, this, &MainForm::onPushButtonAddMonsterClick);
	connect(ui.action_EditMonster, &QAction::triggered, this, &MainForm::onPushButtonEditMonsterClick);
	connect(ui.action_DeleteMonster, &QAction::triggered, this, &MainForm::onPushButtonDeleteMonsterClick);
	connect(ui.pushButtonAddMonster, &QPushButton::clicked, this, &MainForm::onPushButtonAddMonsterClick);
	connect(ui.pushButtonEditMonster, &QPushButton::clicked, this, &MainForm::onPushButtonEditMonsterClick);
	connect(ui.pushButtonDeleteMonster, &QPushButton::clicked, this, &MainForm::onPushButtonDeleteMonsterClick);
	connect(ui.tableWidgetMonsters, &QTableWidget::itemDoubleClicked, this, &MainForm::onTableWidgetMonstersDoubleClicked);
	tableWidgetMonstersKeyWatcher.installOn(ui.tableWidgetMonsters);
	connect(&tableWidgetMonstersKeyWatcher, &QTableWidgetKeyPressWatcher::keyPressed, this, &MainForm::onTableWidgetMonstersKeyPressEvent);
}

void MainForm::functionAfterShown()
{
	setWindowIcon(QIcon(":/MonsterEditor Icon.png"));
}

bool MainForm::event(QEvent *event)
{
	const bool ret_val = QMainWindow::event(event);
	if (!m_functionAfterShownCalled && event->type() == QEvent::Paint) {
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
	if (configManager.load()) {
		configManager.setStringValue(MainForm::THEME_PATH, "");
		setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
		if (!configManager.save()) {
			ErrorMessage::show("An error occurred while saving the configuration file.",
							   configManager.getLastError());
		}
	}
	else {
		ErrorMessage::show("An error occurred while loading the configuration file.",
						   configManager.getLastError());
	}
}

void MainForm::action_DarkTheme_Click()
{
	ConfigurationManager configManager(m_userConfigFolder + "config.json");
	if (configManager.load()) {
		configManager.setStringValue(MainForm::THEME_PATH, "Dark");
		setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
		if (!configManager.save()) {
			ErrorMessage::show("An error occurred while saving the configuration file.",
							   configManager.getLastError());
		}
	}
	else {
		ErrorMessage::show("An error occurred while loading the configuration file.",
						   configManager.getLastError());
	}
}

void MainForm::action_OpenMonsterStore_Click()
{
	QString fullFilePath{QFileDialog::getOpenFileName(this,
													  tr("Open the monster store"),
													  "",
													  tr("Monster store file (*.mon)"))};
	if (fullFilePath != "") {
		openMonsterStore(fullFilePath.toStdString());
		m_currentFilePath = fullFilePath.toStdString();
		refreshMonstersTable();
	}
	refreshWindowTitle();
}

void MainForm::action_OpenRecentMonstersDB_Click()
{
	QAction *recentAction = qobject_cast<QAction *>(sender());
	std::string filename = recentAction->text().toStdString();
	openMonsterStore(filename);
}

void MainForm::action_SaveMonsterStore_Click()
{
	if (m_currentFilePath == "") {
		action_SaveAsMonsterStore_Click();
	}
	else {
		saveMonsterStore(m_currentFilePath);
	}
}

void MainForm::action_SaveAsMonsterStore_Click()
{
	QString filter = "Monster store file (*.mon)";
	QString fullFilePath{QFileDialog::getSaveFileName(this,
													  tr("Save the monster store"),
													  "",
													  filter, &filter)};
	if (fullFilePath != "") {
		m_currentFilePath = fullFilePath.toStdString();
		saveMonsterStore(m_currentFilePath);
		addNewRecentMonstersDB(m_currentFilePath);
	}
	refreshWindowTitle();
}

void MainForm::refreshRecentMapsMenu()
{
	auto recents = std::vector<std::string>{};
	ConfigurationManager configManager(m_userConfigFolder + "config.json");
	if (configManager.load()) {
		recents = configManager.getVectorOfStringValue(MainForm::RECENT_DB);
	}
	else {
		ErrorMessage::show("An error occurred while loading the configuration file.",
						   configManager.getLastError());
		return;
	}

	if (recents.size() > 5) {
		recents.resize(5);
	}
	ui.menu_RecentMonstersDB->setEnabled(!recents.empty());
	std::vector<QAction *> actions{
		ui.action_RecentMap1,
		ui.action_RecentMap2,
		ui.action_RecentMap3,
		ui.action_RecentMap4,
		ui.action_RecentMap5};
	for (size_t i = 0; i < recents.size(); i++) {
		actions[i]->setVisible(true);
		actions[i]->setText(recents[i].c_str());
	}
}

void MainForm::addNewRecentMonstersDB(const std::string &filePath)
{
	auto recents = std::vector<std::string>{};
	// Load existing recent maps
	ConfigurationManager configManager(m_userConfigFolder + "config.json");
	if (configManager.load()) {
		recents = configManager.getVectorOfStringValue(MainForm::RECENT_DB);
	}
	else {
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
	configManager.setVectorOfStringValue(MainForm::RECENT_DB, recents);
	if (!configManager.save()) {
		ErrorMessage::show("An error occurred while saving the configuration file.",
						   configManager.getLastError());
		return;
	}
	refreshRecentMapsMenu();
}

void MainForm::refreshMonstersTable()
{
	ui.tableWidgetMonsters->model()->removeRows(0, ui.tableWidgetMonsters->rowCount());
	auto monstersToDisplay = m_controller.getMonsters();
	std::vector<std::string> monsterIds;
	std::transform(monstersToDisplay.begin(),
									monstersToDisplay.end(),
									std::back_inserter(monsterIds),
									[](const MonsterListDisplay &monsterDisplay) -> std::string {
                                        return monsterDisplay.id; });
	auto monsterIdsWithIcon = m_controller.getIconsFromMonsterIds(monsterIds, getResourcesPath());
	int index = 0;
	for (const auto &monster : monstersToDisplay) {
		ui.tableWidgetMonsters->insertRow(index);
		auto idMonster = new QTableWidgetItem(monster.id.c_str());
		if (monsterIdsWithIcon.find(monster.id) != monsterIdsWithIcon.end()) {
			idMonster->setIcon(monsterIdsWithIcon[monster.id]);
		}
		ui.tableWidgetMonsters->setItem(index, 0, idMonster);
		ui.tableWidgetMonsters->setItem(index, 1, new QTableWidgetItem(monster.name.c_str()));
		ui.tableWidgetMonsters->setItem(index, 2, new QTableWidgetItem(std::to_string(monster.health).c_str()));
		ui.tableWidgetMonsters->setItem(index, 3, new QTableWidgetItem(fmt::format("{}", monster.attack).c_str()));
		ui.tableWidgetMonsters->setItem(index, 4, new QTableWidgetItem(fmt::format("{}", monster.defense).c_str()));
		ui.tableWidgetMonsters->setItem(index, 5, new QTableWidgetItem(fmt::format("[{}-{}]",
                                                                                   monster.goldMinimum,
                                                                                   monster.goldMaximum).c_str()));
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
	if (m_executablePath.empty()) {
		char result[PATH_MAX] = {};
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		if (count != -1) {
			m_executablePath = dirname(result);
		}
	}
	return m_executablePath;
}

const std::string &MainForm::getResourcesPath()
{
	if (m_resourcesPath.empty()) {
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
	if (style == "Dark") {
		QFile file(fmt::format("{0}/darkstyle/darkstyle.qss", getResourcesPath()).c_str());
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

void MainForm::openMonsterStore(const std::string &filePath)
{
	if (!m_controller.openMonsterStore(filePath)) {
		ErrorMessage::show("An error occurred while loading the monster store.",
						   m_controller.getLastError());
		return;
	}
	m_currentFilePath = filePath;
	addNewRecentMonstersDB(m_currentFilePath);
	refreshMonstersTable();
}

void MainForm::saveMonsterStore(const std::string &filePath)
{
	if (!m_controller.saveMonsterStore(filePath)) {
		ErrorMessage::show("An error occurred while saving the monster store.",
						   m_controller.getLastError());
	}
}

void MainForm::refreshWindowTitle()
{
	setWindowTitle(m_currentFilePath.empty() ?
                   "MonsterEditor" :
                   fmt::format("MonsterEditor - {0}", m_currentFilePath).c_str());
}

void MainForm::onTableWidgetMonstersDoubleClicked(QTableWidgetItem *item)
{
	if (item) {
		onPushButtonEditMonsterClick();
	}
}

void MainForm::onTableWidgetMonstersKeyPressEvent(int key, int, int)
{
	if (key == Qt::Key_Delete) {
		onPushButtonDeleteMonsterClick();
	}
}

void MainForm::onPushButtonAddMonsterClick()
{
    EditMonsterForm editMonsterForm(this,
                                    m_resourcesPath,
                                    m_controller.getMonsterStore(),
                                    std::nullopt);
	if (editMonsterForm.exec() == QDialog::Accepted) {
        refreshMonstersTable();
	}
}

void MainForm::onPushButtonEditMonsterClick()
{
    if (auto itemId = getSelectedItemId(); itemId.has_value()) {
        EditMonsterForm editMonsterForm(this,
                                        m_resourcesPath,
                                        m_controller.getMonsterStore(),
                                        itemId);
        if (editMonsterForm.exec() == QDialog::Accepted) {
            refreshMonstersTable();
        }
	}
}

void MainForm::onPushButtonDeleteMonsterClick()
{
    if (auto itemId = getSelectedItemId(); itemId.has_value()) {
		QMessageBox msgBox;
		msgBox.setText(fmt::format("Are you sure you want to delete the monster {0}?", itemId.value()).c_str());
		msgBox.setWindowTitle("Confirmation");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if (msgBox.exec() == QMessageBox::Yes) {
			if (!m_controller.deleteMonster(itemId.value())) {
				ErrorMessage::show("Unable to remove the monster.", m_controller.getLastError());
			}
			refreshMonstersTable();
		}
	}
}

std::optional<std::string> MainForm::getSelectedItemId() const
{
	auto selectedRows = ui.tableWidgetMonsters->selectionModel()->selectedRows();
	if (selectedRows.count() == 1) {
		return selectedRows[0].data().toString().toStdString();
	}
	return std::nullopt;
}

