#include "mainForm_TextureListTabComponent.hpp"
#include "editTextureForm.hpp"
#include "../utils/uiUtils.hpp"
#include <QStyle>
#include <fmt/format.h>

using namespace std;

MainForm_TextureListTabComponent::MainForm_TextureListTabComponent() 
    : glComponent(nullptr),
      listWidgetTextures(nullptr),
      pushButtonAddTexture(nullptr),
      pushButtonEditTexture(nullptr),
      pushButtonDeleteTexture(nullptr)
{
}

void MainForm_TextureListTabComponent::initializeUIObjects(const MainForm_TextureListTabComponent_Objects &objects) 
{
    this->glComponent = objects.glComponent;
    this->listWidgetTextures = objects.listWidgetTextures;
    this->pushButtonAddTexture = objects.pushButtonAddTexture;
    this->pushButtonEditTexture = objects.pushButtonEditTexture;
    this->pushButtonDeleteTexture = objects.pushButtonDeleteTexture;
}

void MainForm_TextureListTabComponent::connectUIActions() 
{
    connect(pushButtonAddTexture, &QPushButton::clicked, this, &MainForm_TextureListTabComponent::onPushButtonAddTextureClick);
	connect(pushButtonEditTexture, &QPushButton::clicked, this, &MainForm_TextureListTabComponent::onPushButtonEditTextureClick);
	connect(pushButtonDeleteTexture, &QPushButton::clicked, this, &MainForm_TextureListTabComponent::onPushButtonDeleteTextureClick);
}

void MainForm_TextureListTabComponent::refreshTextureList() 
{
	listWidgetTextures->model()->removeRows(0, listWidgetTextures->count());
	int index {0};
	for(const auto &texture : glComponent->getTextures()) {
		listWidgetTextures->insertItem(index, texture.getName().c_str());
		index++;
	}
}

boost::optional<const Texture &> MainForm_TextureListTabComponent::getSelectedTextureInTextureList() 
{
	if (listWidgetTextures->selectionModel()->hasSelection()) {
		//Find the selected texture
		auto selectedItemName { listWidgetTextures->selectionModel()->selectedRows()[0].data().toString().toStdString() };
		return glComponent->getTextureByName(selectedItemName);
	}
	else {
		return {};
	}
}

void MainForm_TextureListTabComponent::onPushButtonAddTextureClick() 
{
	glComponent->stopAutoUpdate();
	auto alreadyUsedTextureNames { glComponent->getAlreadyUsedTextureNames() };
	EditTextureForm formEditTexture(this, glComponent->getResourcesPath(), nullptr, glComponent->getAlreadyUsedTextureNames());
    UIUtils::CenterToScreen(&formEditTexture);
	if (formEditTexture.exec() == QDialog::Accepted) {
        emit textureAdded(formEditTexture.getTextureInfo());
	}
	glComponent->startAutoUpdate();
}

void MainForm_TextureListTabComponent::onPushButtonEditTextureClick() 
{
	glComponent->stopAutoUpdate();
	auto selectedTexture = getSelectedTextureInTextureList();
	if (selectedTexture.has_value()) {
		auto alreadyUsedTextureNames = glComponent->getAlreadyUsedTextureNames();
		//Remove the actual selected texture name
		auto iter = std::find(alreadyUsedTextureNames.begin(), alreadyUsedTextureNames.end(), selectedTexture->getName());
		if (iter != alreadyUsedTextureNames.end()) {
			alreadyUsedTextureNames.erase(iter);
		}
		EditTextureForm formEditTexture(this, glComponent->getResourcesPath(), selectedTexture.get_ptr(), alreadyUsedTextureNames);
		if (formEditTexture.exec() == QDialog::Accepted) {
            emit textureUpdated(selectedTexture->getName(), formEditTexture.getTextureInfo());
		}
	}
	glComponent->startAutoUpdate();
}

void MainForm_TextureListTabComponent::onPushButtonDeleteTextureClick() 
{
	auto selectedTexture = getSelectedTextureInTextureList();
	if (selectedTexture.has_value()) {
		QMessageBox msgBox;
		msgBox.setText(fmt::format("Are you sure you want to delete the texture {0}?", selectedTexture->getName()).c_str());
		msgBox.setWindowTitle("Confirmation");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if (msgBox.exec() == QMessageBox::Yes) {
			//Check if the texture is used in the map
			msgBox.setText(fmt::format("The texture {0} is used by some map tiles.\nAre you sure you want to proceed?", selectedTexture->getName()).c_str());
			bool isUsed = glComponent->isTextureUsedInMap(selectedTexture->getName());
			if (!isUsed || (isUsed && msgBox.exec() == QMessageBox::Yes)) {
				emit textureDeleted(selectedTexture->getName());
			}
		}
	}
}