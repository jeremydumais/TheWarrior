#include "imageAttributionsForm.hpp"

using namespace std;

ImageAttributionsForm::ImageAttributionsForm(QWidget *parent)
    : QDialog(parent),
    ui(Ui::imageAttributionsFormClass())
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/ItemEditor Icon.png"));
}
