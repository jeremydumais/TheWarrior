#include "mainForm_TextureListTabComponent.hpp"
#include "editTextureForm.hpp"
#include "uiUtils.hpp"
#include <QStyle>
#include <fmt/format.h>

using namespace std;

MainForm_TextureListTabComponent::MainForm_TextureListTabComponent() 
    : m_glComponent(nullptr),
      m_listWidgetTextures(nullptr),
      m_pushButtonAddTexture(nullptr),
      m_pushButtonEditTexture(nullptr),
      m_pushButtonDeleteTexture(nullptr)
{
}

void MainForm_TextureListTabComponent::initializeUIObjects(const MainForm_TextureListTabComponent_Objects &objects) 
{
    this->m_glComponent = objects.glComponent;
    this->m_listWidgetTextures = objects.listWidgetTextures;
    this->m_pushButtonAddTexture = objects.pushButtonAddTexture;
    this->m_pushButtonEditTexture = objects.pushButtonEditTexture;
    this->m_pushButtonDeleteTexture = objects.pushButtonDeleteTexture;
}

void MainForm_TextureListTabComponent::connectUIActions() 
{
    connect(m_pushButtonAddTexture, &QPushButton::clicked, this, &MainForm_TextureListTabComponent::onPushButtonAddTextureClick);
	connect(m_pushButtonEditTexture, &QPushButton::clicked, this, &MainForm_TextureListTabComponent::onPushButtonEditTextureClick);
	connect(m_pushButtonDeleteTexture, &QPushButton::clicked, this, &MainForm_TextureListTabComponent::onPushButtonDeleteTextureClick);
}

void MainForm_TextureListTabComponent::refreshTextureList() 
{
	m_listWidgetTextures->model()->removeRows(0, m_listWidgetTextures->count());
	int index {0};
	for(const auto &texture : m_glComponent->getTextures()) {
		m_listWidgetTextures->insertItem(index, texture.getName().c_str());
		index++;
	}
}

boost::optional<const Texture &> MainForm_TextureListTabComponent::getSelectedTextureInTextureList() 
{
	if (m_listWidgetTextures->selectionModel()->hasSelection()) {
		//Find the selected texture
		auto selectedItemName { m_listWidgetTextures->selectionModel()->selectedRows()[0].data().toString().toStdString() };
		return m_glComponent->getTextureByName(selectedItemName);
	}
	else {
		return {};
	}
}

void MainForm_TextureListTabComponent::onPushButtonAddTextureClick() 
{
	m_glComponent->stopAutoUpdate();
	auto alreadyUsedTextureNames { m_glComponent->getAlreadyUsedTextureNames() };
	EditTextureForm formEditTexture(this, m_glComponent->getResourcesPath(), nullptr, m_glComponent->getAlreadyUsedTextureNames());
    UIUtils::CenterToScreen(&formEditTexture);
	if (formEditTexture.exec() == QDialog::Accepted) {
        emit textureAdded(formEditTexture.getTextureInfo());
	}
	m_glComponent->startAutoUpdate();
}

void MainForm_TextureListTabComponent::onPushButtonEditTextureClick() 
{
	m_glComponent->stopAutoUpdate();
	auto selectedTexture = getSelectedTextureInTextureList();
	if (selectedTexture.has_value()) {
		auto alreadyUsedTextureNames = m_glComponent->getAlreadyUsedTextureNames();
		//Remove the actual selected texture name
		auto iter = std::find(alreadyUsedTextureNames.begin(), alreadyUsedTextureNames.end(), selectedTexture->getName());
		if (iter != alreadyUsedTextureNames.end()) {
			alreadyUsedTextureNames.erase(iter);
		}
		EditTextureForm formEditTexture(this, m_glComponent->getResourcesPath(), selectedTexture.get_ptr(), alreadyUsedTextureNames);
		if (formEditTexture.exec() == QDialog::Accepted) {
            emit textureUpdated(selectedTexture->getName(), formEditTexture.getTextureInfo());
		}
	}
	m_glComponent->startAutoUpdate();
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
			bool isUsed = m_glComponent->isTextureUsedInMap(selectedTexture->getName());
			if (!isUsed || (isUsed && msgBox.exec() == QMessageBox::Yes)) {
				emit textureDeleted(selectedTexture->getName());
			}
		}
	}
}