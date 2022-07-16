#ifndef MAINFORM_H
#define MAINFORM_H

#include "mainController.hpp"
#include "ui_mainForm.h"
#include "qTableWidgetKeyPressWatcher.h"
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
	static const std::string RECENT_DB;
	const std::string &getExecutablePath();
    const std::string &getResourcesPath();
	void setAppStylesheet(const std::string &style);
};

#endif // MAINFORM_H

