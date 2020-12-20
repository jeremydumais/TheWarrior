#ifndef MAINFORM_H
#define MAINFORM_H

#include "gameMap.hpp"
#include <memory>
#include "ui_mainForm.h"

class MainForm : public QMainWindow
{
Q_OBJECT

public:
	explicit MainForm(QWidget *parent = 0);
	~MainForm();
	bool event(QEvent *event) override;
    void functionAfterShown();
private:
	Ui::MainForm ui;
	std::shared_ptr<GameMap> map;
	bool functionAfterShownCalled;
	void showErrorMessage(const std::string &message,
						  const std::string &internalError) const;
	void setAppStylesheet(const std::string &style);
	void connectUIActions();
	void action_About_Click();
	void action_LightTheme_Click();
	void action_DarkTheme_Click();
	void mapPaint(QPaintEvent *e);
	void resizeEvent(QResizeEvent *);
	void onTileClicked(int tileIndex);
	void refreshTextureList();
	void onPushButtonViewTextureClick();
};

#endif // MAINFORM_H

