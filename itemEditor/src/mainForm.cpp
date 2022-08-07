#include "mainForm.hpp"
#include "aboutBoxForm.hpp"
#include "addItemChooserForm.hpp"
#include "configurationManager.hpp"
#include "editArmorItemForm.hpp"
#include "editItemForm.hpp"
#include "editStatsItemForm.hpp"
#include "editWeaponItemForm.hpp"
#include "errorMessage.hpp"
#include "manageTexturesForm.hpp"
#include "specialFolders.hpp"
#include "weaponItem.hpp"
#include <QtCore/qfile.h>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <libgen.h>		  // dirname
#include <linux/limits.h> // PATH_MAX
#include <memory>
#include <optional>
#include <string>
#include <unistd.h> // readlink
#include <vector>

using namespace itemeditor::controllers;

const std::string MainForm::THEME_PATH{"Display.Theme"};
const std::string MainForm::RECENT_MAPS{"ItemsDB.Recents"};

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
	m_userConfigFolder = SpecialFolders::getAppConfigDirectory("TheWarrior_ItemEditor");
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

	initializeCategoriesTableControl();
	initializeItemsTableControl();
	refreshRecentMapsMenu();
	connectUIActions();
	refreshCategoriesTable();
	refreshItemsTable();
}

void MainForm::initializeCategoriesTableControl()
{
	ui.tableWidgetItemCategories->setFixedWidth(300);
	ui.tableWidgetItemCategories->setHorizontalHeaderItem(0, new QTableWidgetItem("Categories"));
}

void MainForm::initializeItemsTableControl()
{
	ui.tableWidgetItems->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
	ui.tableWidgetItems->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
	ui.tableWidgetItems->setHorizontalHeaderItem(2, new QTableWidgetItem("Description"));
	ui.tableWidgetItems->setColumnWidth(1, 200);
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
	connect(ui.action_AddItem, &QAction::triggered, this, &MainForm::onPushButtonAddItemClick);
	connect(ui.action_AddItem, &QAction::triggered, this, &MainForm::onPushButtonAddItemClick);
	connect(ui.action_EditItem, &QAction::triggered, this, &MainForm::onPushButtonEditItemClick);
	connect(ui.action_DeleteItem, &QAction::triggered, this, &MainForm::onPushButtonDeleteItemClick);
	connect(ui.pushButtonAddItem, &QPushButton::clicked, this, &MainForm::onPushButtonAddItemClick);
	connect(ui.pushButtonEditItem, &QPushButton::clicked, this, &MainForm::onPushButtonEditItemClick);
	connect(ui.pushButtonDeleteItem, &QPushButton::clicked, this, &MainForm::onPushButtonDeleteItemClick);
	connect(ui.action_EditItem, &QAction::triggered, this, &MainForm::onPushButtonEditItemClick);
	connect(ui.action_DeleteItem, &QAction::triggered, this, &MainForm::onPushButtonDeleteItemClick);
	connect(ui.pushButtonAddItem, &QPushButton::clicked, this, &MainForm::onPushButtonAddItemClick);
	connect(ui.pushButtonEditItem, &QPushButton::clicked, this, &MainForm::onPushButtonEditItemClick);
	connect(ui.pushButtonDeleteItem, &QPushButton::clicked, this, &MainForm::onPushButtonDeleteItemClick);
	connect(ui.tableWidgetItemCategories->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainForm::onTableWidgetItemCategoriesSelectionChanged);
	connect(ui.tableWidgetItems, &QTableWidget::itemDoubleClicked, this, &MainForm::onTableWidgetItemsDoubleClicked);
	tableWidgetItemsKeyWatcher.installOn(ui.tableWidgetItems);
	connect(&tableWidgetItemsKeyWatcher, &QTableWidgetKeyPressWatcher::keyPressed, this, &MainForm::onTableWidgetItemsKeyPressEvent);
}

void MainForm::functionAfterShown()
{
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
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
		refreshCategoriesTable();
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
		recents = configManager.getVectorOfStringValue(MainForm::RECENT_MAPS);
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
	ui.menu_RecentItemsDB->setEnabled(!recents.empty());
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
		recents = configManager.getVectorOfStringValue(MainForm::RECENT_MAPS);
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
	configManager.setVectorOfStringValue(MainForm::RECENT_MAPS, recents);
	if (!configManager.save())
	{
		ErrorMessage::show("An error occurred while saving the configuration file.",
						   configManager.getLastError());
		return;
	}
	refreshRecentMapsMenu();
}

void MainForm::refreshCategoriesTable()
{
	// Get the selected category
	std::string selectedCategory = "";
	auto currentCategoryItems = ui.tableWidgetItemCategories->selectedItems();
	if (currentCategoryItems.length() == 1)
	{
		selectedCategory = currentCategoryItems[0]->text().toStdString();
	}
	// Clear and regenerate the category list
	ui.tableWidgetItemCategories->model()->removeRows(0, ui.tableWidgetItemCategories->rowCount());
	int index = 0;
	for (const std::string &category : m_controller.getItemCategories())
	{
		ui.tableWidgetItemCategories->insertRow(index);
		auto categoryRow = new QTableWidgetItem(category.c_str());
		categoryRow->setIcon(getCategoryIcon(category));
		auto font = categoryRow->font();
		font.setPointSize(16);
		categoryRow->setFont(font);
		ui.tableWidgetItemCategories->setItem(index, 0, categoryRow);
		if (category == selectedCategory)
		{
			categoryRow->setSelected(true);
		}
		index++;
	}
}

void MainForm::refreshItemsTable()
{
	ui.tableWidgetItems->model()->removeRows(0, ui.tableWidgetItems->rowCount());
	auto currentItemCategorySelections = ui.tableWidgetItemCategories->selectionModel()->selectedRows();
	if (currentItemCategorySelections.length() == 1)
	{
		auto selectedItemCategory = currentItemCategorySelections[0].data().toString().toStdString();
		auto itemsToDisplay = m_controller.getItemsFromCategory(selectedItemCategory);
		std::vector<std::string> itemIds;
		std::transform(itemsToDisplay.begin(),
									  itemsToDisplay.end(),
									  std::back_inserter(itemIds),
									  [](const ItemListDisplay &itemDisplay) -> std::string { return itemDisplay.id; });
		auto itemIdsWithIcon = m_controller.getIconsFromItemIds(itemIds, getResourcesPath());
		int index = 0;
		for (const auto &item : itemsToDisplay)
		{
			ui.tableWidgetItems->insertRow(index);
			auto idItem = new QTableWidgetItem(item.id.c_str());
			if (itemIdsWithIcon.find(item.id) != itemIdsWithIcon.end()) {
				idItem->setIcon(itemIdsWithIcon[item.id]);
			}
			ui.tableWidgetItems->setItem(index, 0, idItem);
			ui.tableWidgetItems->setItem(index, 1, new QTableWidgetItem(item.name.c_str()));
			ui.tableWidgetItems->setItem(index, 2, new QTableWidgetItem(item.description.c_str()));
			index++;
		}
	}
}

QIcon MainForm::getCategoryIcon(const std::string &categoryName) const
{
	if (categoryName == "Weapon")
	{
		return QIcon(":/sword.png");
	}
	else if (categoryName == "Armor")
	{
		return QIcon(":/shield.png");
	}
	else if (categoryName == "StatsItem")
	{
		return QIcon(":/statsitem.png");
	}
	else
	{
		return QIcon(":/item.png");
	}
}

/*QIcon MainForm::getItemIcon(const std::string &itemId) const
{

}*/

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

void MainForm::openItemStore(const std::string &filePath)
{
	if (!m_controller.openItemStore(filePath))
	{
		ErrorMessage::show("An error occurred while loading the item store.",
						   m_controller.getLastError());
		return;
	}
	m_currentFilePath = filePath;
	addNewRecentItemsDB(m_currentFilePath);
	refreshCategoriesTable();
	refreshItemsTable();
}

void MainForm::saveItemStore(const std::string &filePath)
{
	if (!m_controller.saveItemStore(filePath))
	{
		ErrorMessage::show("An error occurred while saving the item store.",
						   m_controller.getLastError());
	}
}

void MainForm::refreshWindowTitle()
{
	setWindowTitle(m_currentFilePath.empty() ? "ItemEditor" : fmt::format("ItemEditor - {0}", m_currentFilePath).c_str());
}

void MainForm::onTableWidgetItemCategoriesSelectionChanged()
{
	refreshItemsTable();
}

void MainForm::onTableWidgetItemsDoubleClicked(QTableWidgetItem *item)
{
	if (item)
	{
		onPushButtonEditItemClick();
	}
}

void MainForm::onTableWidgetItemsKeyPressEvent(int key, int, int)
{
	if (key == Qt::Key_Delete)
	{
		onPushButtonDeleteItemClick();
	}
}

void MainForm::onPushButtonAddItemClick()
{
	AddItemChooserForm addItemChooserForm(this);
	if (addItemChooserForm.exec() == QDialog::Accepted)
	{
		std::unique_ptr<QDialog> dialog = getItemTypeForm(addItemChooserForm.getResult(),
														  std::nullopt);
		if (dialog != nullptr && dialog->exec() == QDialog::Accepted)
		{
			refreshCategoriesTable();
			refreshItemsTable();
		}
	}
}

void MainForm::onPushButtonEditItemClick()
{
	if (auto itemId = getSelectedItemId(); itemId.has_value())
	{
		auto itemType = m_controller.getItemTypeFromItemId(itemId.value());
		if (itemType.has_value())
		{
			std::unique_ptr<QDialog> dialog = getItemTypeForm(itemType.value(),
															  itemId.value());
			if (dialog != nullptr && dialog->exec() == QDialog::Accepted)
			{
				refreshCategoriesTable();
				refreshItemsTable();
			}
		}
		else
		{
			ErrorMessage::show("Unable to get the type of the selected item.");
		}
	}
}

void MainForm::onPushButtonDeleteItemClick()
{
	if (auto itemId = getSelectedItemId(); itemId.has_value())
	{
		QMessageBox msgBox;
		msgBox.setText(fmt::format("Are you sure you want to delete the item {0}?", itemId.value()).c_str());
		msgBox.setWindowTitle("Confirmation");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if (msgBox.exec() == QMessageBox::Yes)
		{
			if (!m_controller.deleteItem(itemId.value()))
			{
				ErrorMessage::show("Unable to remove the item.", m_controller.getLastError());
			}
			refreshCategoriesTable();
			refreshItemsTable();
		}
	}
}

std::optional<std::string> MainForm::getSelectedItemId() const
{
	auto selectedRows = ui.tableWidgetItems->selectionModel()->selectedRows();
	if (selectedRows.count() == 1) {
		return selectedRows[0].data().toString().toStdString();
	}
	return std::nullopt;
}

std::unique_ptr<QDialog> MainForm::getItemTypeForm(ItemType itemType, std::optional<std::string> itemIdToEdit)
{
	std::unique_ptr<QDialog> dialog = nullptr;
	switch (itemType)
	{
	case ItemType::Item:
		dialog = std::make_unique<EditItemForm>(this,
												getResourcesPath(),
												m_controller.getItemStore(),
												itemIdToEdit);
		break;
	case ItemType::Weapon:
		dialog = std::make_unique<EditWeaponItemForm>(this,
													  getResourcesPath(),
													  m_controller.getItemStore(),
													  itemIdToEdit);
		break;
	case ItemType::Armor:
		dialog = std::make_unique<EditArmorItemForm>(this,
													 getResourcesPath(),
													 m_controller.getItemStore(),
													 itemIdToEdit);
		break;
	case ItemType::StatsItem:
		dialog = std::make_unique<EditStatsItemForm>(this,
													 getResourcesPath(),
													 m_controller.getItemStore(),
													 itemIdToEdit);
		break;
	default:
		break;
	}
	return dialog;
}
