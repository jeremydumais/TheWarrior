#include "mainForm_TextureSelectionComponent.hpp"
#include <fmt/format.h>

using namespace std;

MainForm_TextureSelectionComponent::MainForm_TextureSelectionComponent() 
    : m_glComponent(nullptr),
      m_comboBoxTexture(nullptr),
      m_labelSelectedTexture(nullptr),
      m_pushButtonSelectedTextureClear(nullptr),
      m_labelSelectedObject(nullptr),
      m_pushButtonSelectedObjectClear(nullptr),
      m_labelImageTexture(nullptr)
{
}

void MainForm_TextureSelectionComponent::initializeUIObjects(const MainForm_TextureSelectionComponent_Objects &objects) 
{
    this->m_glComponent = objects.glComponent;
    this->m_comboBoxTexture = objects.comboBoxTexture;
    this->m_labelSelectedTexture = objects.labelSelectedTexture;
    this->m_pushButtonSelectedTextureClear = objects.pushButtonSelectedTextureClear;
    this->m_labelSelectedObject = objects.labelSelectedObject;
    this->m_pushButtonSelectedObjectClear = objects.pushButtonSelectedObjectClear;
    this->m_labelImageTexture = objects.labelImageTexture;
}

void MainForm_TextureSelectionComponent::connectUIActions() 
{
    connect(m_pushButtonSelectedTextureClear, &QPushButton::clicked, this, &MainForm_TextureSelectionComponent::onPushButtonSelectedTextureClearClick);
	connect(m_pushButtonSelectedObjectClear, &QPushButton::clicked, this, &MainForm_TextureSelectionComponent::onPushButtonSelectedObjectClearClick);
	connect(m_labelImageTexture, &QClickableLabel::onMouseReleaseEvent, this, &MainForm_TextureSelectionComponent::onLabelImageTextureMouseReleaseEvent);
	connect(m_comboBoxTexture, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainForm_TextureSelectionComponent::onComboBoxTextureCurrentIndexChanged);
}

void MainForm_TextureSelectionComponent::refreshTextureList() 
{
	m_comboBoxTexture->model()->removeRows(0, m_comboBoxTexture->count());
	int index {0};
	for(const auto &texture : m_glComponent->getTextures()) {
		m_comboBoxTexture->insertItem(index, texture.getName().c_str());
		index++;
	}
	displaySelectedTextureImage();
}

void MainForm_TextureSelectionComponent::displaySelectedTextureImage() 
{
	//Find the selected texture
	auto texture { m_glComponent->getTextureByName(m_comboBoxTexture->itemText(m_comboBoxTexture->currentIndex()).toStdString()) };
	if (texture.has_value()) {
		QImageReader reader(fmt::format("{0}/{1}", m_glComponent->getResourcesPath(), texture->get().getFilename()).c_str());
		const QImage image { reader.read() };
		m_labelImageTexture->setFixedSize(image.width(), image.height());
		m_labelImageTexture->setPixmap(QPixmap::fromImage(image));
	}
	else {
		m_labelImageTexture->clear();
		m_glComponent->clearLastSelectedTexture();
		m_glComponent->clearLastSelectedObject();
		m_labelSelectedTexture->clear();
		m_labelSelectedObject->clear();
	}
}

QPixmap MainForm_TextureSelectionComponent::getTextureTileImageFromTexture(int tileIndex, const Texture &texture) const 
{
	int textureWidthInPixel { texture.getWidth() };
	int textureHeightInPixel { texture.getHeight() };
	int x { (tileIndex * texture.getTileWidth()) % textureWidthInPixel };
	int y { textureHeightInPixel - (((tileIndex * texture.getTileWidth()) / textureWidthInPixel) * texture.getTileHeight()) };
	QPixmap imagePart = m_labelImageTexture->pixmap(Qt::ReturnByValue).copy(x, 
																			y - texture.getTileHeight(), 
																			texture.getTileWidth(), 
																			texture.getTileHeight());
	return imagePart;
}

void MainForm_TextureSelectionComponent::onPushButtonSelectedTextureClearClick() 
{
	m_glComponent->clearLastSelectedTexture();
	m_labelSelectedTexture->clear();
}

void MainForm_TextureSelectionComponent::onPushButtonSelectedObjectClearClick() 
{
	m_glComponent->clearLastSelectedObject();
	m_labelSelectedObject->clear();
}

void MainForm_TextureSelectionComponent::onLabelImageTextureMouseReleaseEvent(QMouseEvent *event) 
{
	int comboBoxTextureCurrentIndex { m_comboBoxTexture->currentIndex() };
	string textureName { m_comboBoxTexture->itemText(comboBoxTextureCurrentIndex).toStdString() };
	auto texture { m_glComponent->getTextureByName(textureName) };
	if (texture.has_value()) {
		string name { textureName };
		int index = m_controller.getTextureIndexFromPosition(Point(event->pos().x(), event->pos().y()), texture->get());
		//Display the selected texture or object on the selected image
		auto imagePart { getTextureTileImageFromTexture(index, texture->get()) };
        auto selectionMode { m_glComponent->getSelectionMode() };
		if (selectionMode == SelectionMode::ApplyTexture) {
			m_glComponent->setLastSelectedTexture(name, index);
			m_labelSelectedTexture->setPixmap(imagePart);
		}
		else if (selectionMode == SelectionMode::ApplyObject) {
			m_glComponent->setLastSelectedObject(name, index);
			m_labelSelectedObject->setPixmap(imagePart);
		}
	}	
}

void MainForm_TextureSelectionComponent::onComboBoxTextureCurrentIndexChanged() 
{
	displaySelectedTextureImage();
}