#ifndef MAINFORM_H
#define MAINFORM_H

#include "mainController.hpp"
#include "ui_mainForm.h"
#include "qTableWidgetKeyPressWatcher.h"
#include <memory>
#include <optional>
#include <string>

class MainForm : public QMainWindow
{
Q_OBJECT

public:
	explicit MainForm(QWidget *parent = 0);
	~MainForm() = default;
	void connectUIActions();
	bool event(QEvent *event) override;
    void functionAfterShown();
private:
	Ui::MainForm ui;
	bool m_functionAfterShownCalled;
	std::string m_userConfigFolder;
	std::string m_executablePath;
	std::string m_resourcesPath;
	std::string m_currentFilePath;
	MainController m_controller;
	QTableWidgetKeyPressWatcher tableWidgetItemsKeyWatcher;
	static const std::string THEME_PATH;
	static const std::string RECENT_MAPS;
	void initializeCategoriesTableControl();
	void initializeItemsTableControl();
	void action_About_Click();
	void action_LightTheme_Click();
	void action_DarkTheme_Click();
	void action_OpenItemStore_Click();
	void action_SaveItemStore_Click();
	void action_SaveAsItemStore_Click();
	void refreshCategoriesTable();
	void refreshItemsTable();
	QIcon getCategoryIcon(const std::string &categoryName) const;
	void action_ManageTextures_Click();
    const std::string &getExecutablePath();
    const std::string &getResourcesPath();
	void setAppStylesheet(const std::string &style);
	void openItemStore(const std::string &filePath);
	void saveItemStore(const std::string &filePath);
	void refreshWindowTitle();
	void onTableWidgetItemCategoriesSelectionChanged();
	void onTableWidgetItemsDoubleClicked(QTableWidgetItem *item);
	void onTableWidgetItemsKeyPressEvent(int key, int, int);
	void onPushButtonAddItemClick();
	void onPushButtonEditItemClick();
	void onPushButtonDeleteItemClick();
	std::optional<std::string> getSelectedItemId() const;	
	std::unique_ptr<QDialog> getItemTypeForm(ItemType itemType, std::optional<std::string> itemIdToEdit);
};

#endif // MAINFORM_H

