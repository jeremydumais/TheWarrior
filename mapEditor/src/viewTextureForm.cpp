#include "viewTextureForm.h"

using namespace std;

ViewTextureForm::ViewTextureForm(QWidget *parent)
	: QDialog(parent),
	  ui(Ui::viewTextureFormClass())
{
	ui.setupUi(this);
}