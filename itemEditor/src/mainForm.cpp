#include "mainForm.hpp"
#include "aboutBoxForm.hpp"
#include "addItemForm.hpp"
#include "addItemChooserForm.hpp"
#include "configurationManager.hpp"
#include "errorMessage.hpp"
#include "manageTexturesForm.hpp"
#include "specialFolders.hpp"
#include <QtCore/qfile.h>
#include <QtWidgets/QFileDialog>
#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <libgen.h>         // dirname
#include <linux/limits.h>   // PATH_MAX
#include <unistd.h>         // readlink

const std::string MainForm::THEME_PATH { "Display.Theme" };
const std::string MainForm::RECENT_MAPS { "ItemsDB.Recents" };

MainForm::MainForm(QWidget *parent)
	: QMainWindow(parent),
	  ui(Ui::MainForm()),
	  m_functionAfterShownCalled(false),
	  m_userConfigFolder(""),
	  m_executablePath(""),
	  m_resourcesPath(""),
	  m_currentFilePath("/home/jed/Programming/TheWarrior/resources/items/itemstore.itm"),
	  m_controller(MainController())
{
	ui.setupUi(this);

    //Check if the user configuration folder exist
	m_userConfigFolder = SpecialFolders::getAppConfigDirectory("TheWarrior_ItemEditor");
	if (!boost::filesystem::exists(m_userConfigFolder)) {
		if (!boost::filesystem::create_directory(m_userConfigFolder)) {
			ErrorMessage::show(fmt::format("Unable to create the folder {0}", m_userConfigFolder), "");
			exit(1);
		}
	}

	//Check if the configuration file exist
	ConfigurationManager configManager(m_userConfigFolder + "config.json");
	if (configManager.load()) {
		setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
	}
	else {
		ErrorMessage::show("An error occurred while loading the configuration file.",
						   configManager.getLastError());
	}

    ui.listWidgetItemCategories->setFixedWidth(300);

    connectUIActions();
	openItemStore(m_currentFilePath);
}

void MainForm::connectUIActions() 
{
	connect(ui.action_Quit, &QAction::triggered, this, &MainForm::close);
    connect(ui.action_LightTheme, &QAction::triggered, this, &MainForm::action_LightTheme_Click);
	connect(ui.action_DarkTheme, &QAction::triggered, this, &MainForm::action_DarkTheme_Click);
	connect(ui.action_About, &QAction::triggered, this, &MainForm::action_About_Click);
	connect(ui.action_Open, &QAction::triggered, this, &MainForm::action_OpenItemStore_Click);
	connect(ui.action_Save, &QAction::triggered, this, &MainForm::action_SaveItemStore_Click);
	connect(ui.action_SaveAs, &QAction::triggered, this, &MainForm::action_SaveAsItemStore_Click);
	connect(ui.action_ManageTextures, &QAction::triggered, this, &MainForm::action_ManageTextures_Click);
	connect(ui.pushButtonAddItem, &QPushButton::clicked, this, &MainForm::onPushButtonAddAnItemClick);
}

void MainForm::functionAfterShown()
{
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
}

bool MainForm::event(QEvent *event)
{
    const bool ret_val = QMainWindow::event(event);
    if(!m_functionAfterShownCalled && event->type() == QEvent::Paint)
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

void MainForm::action_OpenItemStore_Click() 
{
	QString fullFilePath { QFileDialog::getOpenFileName(this, 
							tr("Open the item store"),
							"",
							tr("Item store file (*.itm)")) };
	if (fullFilePath != "") {
		openItemStore(fullFilePath.toStdString());
		m_currentFilePath = fullFilePath.toStdString();
	}
	refreshWindowTitle();
}

void MainForm::action_SaveItemStore_Click() 
{
	if (m_currentFilePath == "") {
		action_SaveAsItemStore_Click();
	}
	else {
		saveItemStore(m_currentFilePath);
	}
}

void MainForm::action_SaveAsItemStore_Click() 
{
	QString filter = "Item store file (*.itm)";
	QString fullFilePath { QFileDialog::getSaveFileName(this, 
							tr("Save the item store"),
							"",
							filter, &filter) };
	if (fullFilePath != "") {
		m_currentFilePath = fullFilePath.toStdString();
		saveItemStore(m_currentFilePath);
	}
	refreshWindowTitle();
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
		char result[PATH_MAX];
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
	/*
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

void MainForm::openItemStore(const std::string &filePath) 
{
	if (!m_controller.openItemStore(filePath)) {
		ErrorMessage::show("An error occurred while loading the item store.",
						   m_controller.getLastError());
	}
}

void MainForm::saveItemStore(const std::string &filePath) 
{
	if (!m_controller.saveItemStore(filePath)) {
		ErrorMessage::show("An error occurred while saving the item store.",
						   m_controller.getLastError());
	}
}

void MainForm::refreshWindowTitle() 
{
	setWindowTitle(m_currentFilePath.empty() ? 
				   "ItemEditor" : 
				   fmt::format("ItemEditor - {0}", m_currentFilePath).c_str());
}

void MainForm::onPushButtonAddAnItemClick() 
{
	AddItemChooserForm addItemChooserForm(this);
	if (addItemChooserForm.exec() == QDialog::Accepted) {
		AddItemForm addItemForm(this,
								m_resourcesPath,
								m_controller.getItemStore());
		addItemForm.exec();
	}
}