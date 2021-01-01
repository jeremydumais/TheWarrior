#include "aboutBoxForm.hpp"
#include "imageAttributionsForm.hpp"

using namespace std;

AboutBoxForm::AboutBoxForm(QWidget *parent)
	: QDialog(parent),
	  ui(Ui::aboutBoxFormClass())
{
	ui.setupUi(this);
    connect(ui.pushButtonImageAttributions, &QPushButton::clicked, this, &AboutBoxForm::pushButtonImageAttributions_Click);
	setWindowIcon(QIcon(":/MapEditor Icon.png"));
}


void AboutBoxForm::pushButtonImageAttributions_Click() 
{
	ImageAttributionsForm formImageAttributions(this);
	formImageAttributions.exec();
}