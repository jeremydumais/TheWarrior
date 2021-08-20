#ifndef MAINFORM_H
#define MAINFORM_H

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
	static const std::string THEME_PATH;
	static const std::string RECENT_MAPS;
	void action_About_Click();
	void action_LightTheme_Click();
	void action_DarkTheme_Click();
    const std::string &getExecutablePath();
    const std::string &getResourcesPath();
	void setAppStylesheet(const std::string &style);
};

#endif // MAINFORM_H

