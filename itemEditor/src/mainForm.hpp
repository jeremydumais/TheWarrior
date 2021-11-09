#ifndef MAINFORM_H
#define MAINFORM_H

#include "mainController.hpp"
#include "ui_mainForm.h"
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
	bool functionAfterShownCalled;
	std::string userConfigFolder;
	std::string executablePath;
	std::string resourcesPath;
	std::string currentFilePath;
	MainController controller;
	static const std::string THEME_PATH;
	static const std::string RECENT_MAPS;
	void action_About_Click();
	void action_LightTheme_Click();
	void action_DarkTheme_Click();
	void action_OpenItemStore_Click();
	void action_SaveItemStore_Click();
	void action_SaveAsItemStore_Click();
	void action_ManageTextures_Click();
    const std::string &getExecutablePath();
    const std::string &getResourcesPath();
	void setAppStylesheet(const std::string &style);
	void openItemStore(const std::string &filePath);
	void saveItemStore(const std::string &filePath);
	void refreshWindowTitle();
};

#endif // MAINFORM_H

