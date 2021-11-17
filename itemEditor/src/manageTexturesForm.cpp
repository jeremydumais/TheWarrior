#include "manageTexturesForm.hpp"
#include "editTextureForm.hpp"
#include "errorMessage.hpp"
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
	  m_resourcesPath(resourcesPath),
	  m_controller(ManageTextureController(textureContainer))
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
	connect(ui.pushButtonEditTexture, &QPushButton::clicked, this, &ManageTexturesForm::onPushButtonEditClick);
}

void ManageTexturesForm::refreshTextureList() 
{
	ui.listWidgetTextures->model()->removeRows(0, ui.listWidgetTextures->count());
	int index {0};
	for(const auto &texture : m_controller.getTextures()) {
        ui.listWidgetTextures->insertItem(index, texture.getName().c_str());
        index++;
	}
}

void ManageTexturesForm::onPushButtonCloseClick() 
{
	close();
}

optional<reference_wrapper<const Texture>> ManageTexturesForm::getSelectedTextureInTextureList() 
{
	if (ui.listWidgetTextures->selectionModel()->hasSelection()) {
		//Find the selected texture
		auto selectedItemName { ui.listWidgetTextures->selectionModel()->selectedRows()[0].data().toString().toStdString() };
		return m_controller.getTextureByName(selectedItemName);
	}
	else {
		return nullopt;
	}
}

void ManageTexturesForm::onPushButtonAddClick() 
{
	EditTextureForm editTextureForm(this,
									m_resourcesPath,
									nullptr,
									m_controller.getAlreadyUsedNames());
	if (editTextureForm.exec() == QDialog::Accepted) {
		//Add the new texture
		if (!m_controller.addTexture(editTextureForm.getTextureInfo())) {
			ErrorMessage::show("Unable to add the texture.");
		}
		refreshTextureList();
	}
}

void ManageTexturesForm::onPushButtonEditClick() 
{
 	auto selectedTexture = getSelectedTextureInTextureList();
	if (selectedTexture.has_value()) {
		auto alreadyUsedTextureNames = m_controller.getAlreadyUsedNames();
		//Remove the actual selected texture name
		auto iter = std::find(alreadyUsedTextureNames.begin(), alreadyUsedTextureNames.end(), selectedTexture->get().getName());
		if (iter != alreadyUsedTextureNames.end()) {
				alreadyUsedTextureNames.erase(iter);
		}
		EditTextureForm formEditTexture(this, m_resourcesPath, &selectedTexture->get(), alreadyUsedTextureNames);
		if (formEditTexture.exec() == QDialog::Accepted) {
			if (!m_controller.replaceTexture(selectedTexture->get().getName(), formEditTexture.getTextureInfo())) {
				ErrorMessage::show("Unable to replace the texture.");
			}
			refreshTextureList();
			//emit textureUpdated(selectedTexture->get().getName(), formEditTexture.getTextureInfo());
		}
	}



	/*EditTextureForm editTextureForm(this,
									m_resourcesPath,
									nullptr,
									m_controller.getAlreadyUsedNames());
	if (editTextureForm.exec() == QDialog::Accepted) {
		//Add the new texture
		m_controller.addTexture(editTextureForm.getTextureInfo());
		refreshTextureList();
	}*/
}