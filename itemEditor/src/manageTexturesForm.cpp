#include "manageTexturesForm.hpp"
#include "editTextureForm.hpp"
#include "errorMessage.hpp"
#include <QMessageBox>
#include <QIcon>
#include <fmt/format.h>
#include <memory>

ManageTexturesForm::ManageTexturesForm(QWidget *parent,
								       const std::string &resourcesPath,
								       TextureContainer &textureContainer)
	: QDialog(parent),
	  ui(Ui::manageTexturesFormClass()),
	  m_resourcesPath(resourcesPath),
	  m_controller(ManageTextureController(textureContainer))
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
	connectUIActions();
	refreshTextureList();
}

void ManageTexturesForm::connectUIActions() 
{
	connect(ui.pushButtonClose, &QPushButton::clicked, this, &ManageTexturesForm::onPushButtonCloseClick);
	connect(ui.pushButtonAddTexture, &QPushButton::clicked, this, &ManageTexturesForm::onPushButtonAddClick);
	connect(ui.pushButtonEditTexture, &QPushButton::clicked, this, &ManageTexturesForm::onPushButtonEditClick);
	connect(ui.pushButtonDeleteTexture, &QPushButton::clicked, this, &ManageTexturesForm::onPushButtonDeleteClick);
}

void ManageTexturesForm::refreshTextureList() 
{
	ui.listWidgetTextures->model()->removeRows(0, ui.listWidgetTextures->count());
	int index {0};
	for(const auto &textureName : m_controller.getTexturesNames()) {
        ui.listWidgetTextures->insertItem(index, textureName.c_str());
        index++;
	}
}

void ManageTexturesForm::onPushButtonCloseClick() 
{
	close();
}

std::unique_ptr<TextureDTO> ManageTexturesForm::getSelectedTextureInTextureList() 
{
	if (ui.listWidgetTextures->selectionModel()->hasSelection()) {
		//Find the selected texture
		auto selectedItemName { ui.listWidgetTextures->selectionModel()->selectedRows()[0].data().toString().toStdString() };
		return m_controller.getTextureByName(selectedItemName);
	}
	else {
		return nullptr;
	}
}

void ManageTexturesForm::onPushButtonAddClick() 
{
	auto allTextureNames = m_controller.getTexturesNames();
	EditTextureForm editTextureForm(this,
									m_resourcesPath,
									nullptr,
									allTextureNames);
	if (editTextureForm.exec() == QDialog::Accepted) {
		if (!m_controller.addTexture(std::move(editTextureForm.getTextureInfo()))) {
			ErrorMessage::show("Unable to add the texture.");
		}
		refreshTextureList();
	}
}

void ManageTexturesForm::onPushButtonEditClick() 
{
 	auto selectedTexture = getSelectedTextureInTextureList();
	if (selectedTexture != nullptr) {
		auto allTextureNames = m_controller.getTexturesNames();
		auto originalName = selectedTexture->name;
		EditTextureForm formEditTexture(this, m_resourcesPath, std::move(selectedTexture), allTextureNames);
		if (formEditTexture.exec() == QDialog::Accepted) {
			if (!m_controller.replaceTexture(originalName, std::move(formEditTexture.getTextureInfo()))) {
				ErrorMessage::show("Unable to replace the texture.");
			}
			refreshTextureList();
		}
	}
}

void ManageTexturesForm::onPushButtonDeleteClick() 
{
	auto selectedTexture = getSelectedTextureInTextureList();
	if (selectedTexture != nullptr) {
		QMessageBox msgBox;
		msgBox.setText(fmt::format("Are you sure you want to delete the texture {0}?", selectedTexture->name).c_str());
		msgBox.setWindowTitle("Confirmation");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if (msgBox.exec() == QMessageBox::Yes) {
			if (!m_controller.removeTexture(selectedTexture->name)) {
				ErrorMessage::show("Unable to remove the texture.");
			}
			refreshTextureList();
		}
    }
}