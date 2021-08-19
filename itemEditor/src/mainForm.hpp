#ifndef MAINFORM_H
#define MAINFORM_H

#include "ui_mainForm.h"

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
};

#endif // MAINFORM_H

