#include "manageTexturesForm.hpp"
#include "editTextureForm.hpp"
#include <QMessageBox>
#include <QIcon>
#include <algorithm>
#include <fmt/format.h>

using namespace std;

ManageTexturesForm::ManageTexturesForm(QWidget *parent,
								       const std::string &resourcesPath,
								       TextureContainer &textureContainer)
	: QDialog(parent),
	  ui(Ui::manageTexturesFormClass()),
	  resourcesPath(resourcesPath),
	  controller(ManageTextureController(textureContainer))
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/MapEditor Icon.png"));
	connectUIActions();
	refreshTextureList();
}

void ManageTexturesForm::connectUIActions() 
{
	connect(ui.pushButtonClose, &QPushButton::clicked, this, &ManageTexturesForm::onPushButtonCloseClick);
	connect(ui.pushButtonAddTexture, &QPushButton::clicked, this, &ManageTexturesForm::onPushButtonAddClick);
}

void ManageTexturesForm::refreshTextureList() 
{
	ui.listWidgetTextures->model()->removeRows(0, ui.listWidgetTextures->count());
	int index {0};
	for(const auto &texture : controller.getTextures()) {
        ui.listWidgetTextures->insertItem(index, texture.getName().c_str());
        index++;
	}
}

void ManageTexturesForm::onPushButtonCloseClick() 
{
	close();
}

void ManageTexturesForm::onPushButtonAddClick() 
{
	EditTextureForm editTextureForm(this,
									resourcesPath,
									nullptr,
									controller.getAlreadyUsedNames());
}