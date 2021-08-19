#include "editTileActionChangeMapPropertiesForm.hpp"
#include "errorMessage.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>

using namespace std;

EditTileActionChangeMapPropertiesForm::EditTileActionChangeMapPropertiesForm(QWidget *parent,
								 const std::string &resourcesPath,
								 const std::map<std::string, std::string> &properties)
	: QDialog(parent),
	  ui(Ui::editTileActionChangeMapPropertiesFormClass()),
	  resourcesPath(resourcesPath),
	  properties(properties)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/MapEditor Icon.png"));
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditTileActionChangeMapPropertiesForm::onPushButtonOK);
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditTileActionChangeMapPropertiesForm::reject);
	connect(ui.pushButtonOpenMapFile, &QPushButton::clicked, this, &EditTileActionChangeMapPropertiesForm::onPushButtonOpenMapFileClick);

	ui.comboBoxPlayerFacing->model()->removeRows(0, ui.comboBoxPlayerFacing->count());
	ui.comboBoxPlayerFacing->insertItem(0, "Up");
	ui.comboBoxPlayerFacing->insertItem(1, "Down");
	ui.comboBoxPlayerFacing->insertItem(2, "Left");
	ui.comboBoxPlayerFacing->insertItem(3, "Right");

	if (this->properties.find("mapFileName") != this->properties.end()) {
		ui.lineEditMapFileName->setText(this->properties.at("mapFileName").c_str());
	}
	if (this->properties.find("playerX") != this->properties.end()) {
		ui.spinBoxPlayerX->setValue(stoi(this->properties.at("playerX")));
	}
	if (this->properties.find("playerY") != this->properties.end()) {
		ui.spinBoxPlayerY->setValue(stoi(this->properties.at("playerY")));
	}
	if (this->properties.find("playerFacing") != this->properties.end()) {
		ui.comboBoxPlayerFacing->setCurrentIndex(stoi(this->properties.at("playerFacing")));
	}
}

const std::map<std::string, std::string> &EditTileActionChangeMapPropertiesForm::getUpdatedProperties() const
{
	return properties;
}

void EditTileActionChangeMapPropertiesForm::onPushButtonOK() 
{
	if (ui.lineEditMapFileName->text().isEmpty()) {
		ErrorMessage::show("The map file cannot be empty.");
		return;
	}

	properties.clear();
	properties["mapFileName"] = ui.lineEditMapFileName->text().toStdString();
	properties["playerX"] = to_string(ui.spinBoxPlayerX->value());
	properties["playerY"] = to_string(ui.spinBoxPlayerY->value());
	properties["playerFacing"] = to_string(ui.comboBoxPlayerFacing->currentIndex());
	accept();
}

void EditTileActionChangeMapPropertiesForm::onPushButtonOpenMapFileClick() 
{
	QString fullFilePath { QFileDialog::getOpenFileName(this, 
														tr("Open Map"), 
														resourcesPath.c_str(), 
														tr("Map file (*.map)")) };
	QFileInfo fileInfo(fullFilePath);
	string filename { fileInfo.fileName().toStdString() };
	ui.lineEditMapFileName->setText(filename.c_str());
}