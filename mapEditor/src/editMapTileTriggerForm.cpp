#include "editMapTileTriggerForm.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>

using namespace std;

EditMapTileTriggerForm::EditMapTileTriggerForm(QWidget *parent)
	: QDialog(parent),
	  ui(Ui::editMapTileTriggerFormClass())
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/MapEditor Icon.png"));
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditMapTileTriggerForm::onPushButtonOK);
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditMapTileTriggerForm::reject);

	if (!isEditMode) {
		this->setWindowTitle("Add map tile trigger");
	}
	else {
		this->setWindowTitle("Edit map tile trigger");
	}
}

void EditMapTileTriggerForm::onPushButtonOK() 
{

}

void EditMapTileTriggerForm::showErrorMessage(const std::string &message) const
{
	QMessageBox msgBox;
	msgBox.setText(message.c_str());
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setWindowTitle("Error");
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();
}