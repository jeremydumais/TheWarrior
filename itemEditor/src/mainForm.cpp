#include "mainForm.hpp"

using namespace std;

MainForm::MainForm(QWidget *parent)
	: QMainWindow(parent),
	  ui(Ui::MainForm()),
	  functionAfterShownCalled(false)
{
	ui.setupUi(this);
}

void MainForm::connectUIActions() 
{
}

void MainForm::functionAfterShown()
{
	//setWindowIcon(QIcon(":/MapEditor Icon.png"));
}

bool MainForm::event(QEvent *event)
{
    const bool ret_val = QMainWindow::event(event);
    if(!functionAfterShownCalled && event->type() == QEvent::Paint)
    {
        functionAfterShownCalled = true;
        functionAfterShown();
    }
    return ret_val;
}
