#include "editTileActionOpenChestPropertiesForm.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>

using namespace std;

EditTileActionOpenChestPropertiesForm::EditTileActionOpenChestPropertiesForm(QWidget *parent,
								 const std::map<std::string, std::string> &properties)
	: QDialog(parent),
	  ui(Ui::editTileActionOpenChestPropertiesFormClass()),
	  properties(properties)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/MapEditor Icon.png"));
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditTileActionOpenChestPropertiesForm::onPushButtonOK);
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditTileActionOpenChestPropertiesForm::reject);

	if (this->properties.find("objectTextureIndexOpenedChest") != this->properties.end()) {
		ui.spinBoxObjectTextureIndexOpenedChest->setValue(stoi(this->properties.at("objectTextureIndexOpenedChest")));
	}
}

const std::map<std::string, std::string> &EditTileActionOpenChestPropertiesForm::getUpdatedProperties() const
{
	return properties;
}

void EditTileActionOpenChestPropertiesForm::onPushButtonOK() 
{
	properties.clear();
	properties["objectTextureIndexOpenedChest"] = to_string(ui.spinBoxObjectTextureIndexOpenedChest->value());
	accept();
}