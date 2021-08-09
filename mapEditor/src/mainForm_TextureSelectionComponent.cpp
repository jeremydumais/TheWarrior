#include "mainForm_TextureSelectionComponent.hpp"
#include <fmt/format.h>

using namespace std;

MainForm_TextureSelectionComponent::MainForm_TextureSelectionComponent() 
    : glComponent(nullptr),
      comboBoxTexture(nullptr),
      labelSelectedTexture(nullptr),
      pushButtonSelectedTextureClear(nullptr),
      labelSelectedObject(nullptr),
      pushButtonSelectedObjectClear(nullptr),
      labelImageTexture(nullptr)
{
}

void MainForm_TextureSelectionComponent::initializeUIObjects(const MainForm_TextureSelectionComponent_Objects &objects) 
{
    this->glComponent = objects.glComponent;
    this->comboBoxTexture = objects.comboBoxTexture;
    this->labelSelectedTexture = objects.labelSelectedTexture;
    this->pushButtonSelectedTextureClear = objects.pushButtonSelectedTextureClear;
    this->labelSelectedObject = objects.labelSelectedObject;
    this->pushButtonSelectedObjectClear = objects.pushButtonSelectedObjectClear;
    this->labelImageTexture = objects.labelImageTexture;
}

void MainForm_TextureSelectionComponent::connectUIActions() 
{
    connect(pushButtonSelectedTextureClear, &QPushButton::clicked, this, &MainForm_TextureSelectionComponent::onPushButtonSelectedTextureClearClick);
	connect(pushButtonSelectedObjectClear, &QPushButton::clicked, this, &MainForm_TextureSelectionComponent::onPushButtonSelectedObjectClearClick);
	connect(labelImageTexture, &QClickableLabel::onMouseReleaseEvent, this, &MainForm_TextureSelectionComponent::onLabelImageTextureMouseReleaseEvent);
	connect(comboBoxTexture, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainForm_TextureSelectionComponent::onComboBoxTextureCurrentIndexChanged);
}

void MainForm_TextureSelectionComponent::refreshTextureList() 
{
	comboBoxTexture->model()->removeRows(0, comboBoxTexture->count());
	int index {0};
	for(const auto &texture : glComponent->getTextures()) {
		comboBoxTexture->insertItem(index, texture.getName().c_str());
		index++;
	}
	displaySelectedTextureImage();
}

void MainForm_TextureSelectionComponent::displaySelectedTextureImage() 
{
	//Find the selected texture
	auto texture { glComponent->getTextureByName(comboBoxTexture->itemText(comboBoxTexture->currentIndex()).toStdString()) };
	if (texture.has_value()) {
		QImageReader reader(fmt::format("{0}/{1}", glComponent->getResourcesPath(), texture->getFilename()).c_str());
		const QImage image { reader.read() };
		labelImageTexture->setFixedSize(image.width(), image.height());
		labelImageTexture->setPixmap(QPixmap::fromImage(image));
	}
	else {
		labelImageTexture->clear();
		glComponent->clearLastSelectedTexture();
		glComponent->clearLastSelectedObject();
		labelSelectedTexture->clear();
		labelSelectedObject->clear();
	}
}

int MainForm_TextureSelectionComponent::getTextureIndexFromPosition(const Point &pos, const Texture &texture) 
{
	int realY { texture.getHeight() - pos.y()};
	int indexX = pos.x() / texture.getTileWidth();
	int indexY = realY / texture.getTileHeight();
	int tileIndex { indexX + (indexY * (texture.getWidth()/texture.getTileWidth())) };
	return tileIndex;
}

QPixmap MainForm_TextureSelectionComponent::getTextureTileImageFromTexture(int tileIndex, const Texture &texture) const 
{
	int textureWidthInPixel { texture.getWidth() };
	int textureHeightInPixel { texture.getHeight() };
	int x { (tileIndex * texture.getTileWidth()) % textureWidthInPixel };
	int y { textureHeightInPixel - (((tileIndex * texture.getTileWidth()) / textureWidthInPixel) * texture.getTileHeight()) };
	QPixmap imagePart = labelImageTexture->pixmap()->copy(x, y - texture.getTileHeight(), texture.getTileWidth(), texture.getTileHeight());
	return imagePart;
}

void MainForm_TextureSelectionComponent::onPushButtonSelectedTextureClearClick() 
{
	glComponent->clearLastSelectedTexture();
	labelSelectedTexture->clear();
}

void MainForm_TextureSelectionComponent::onPushButtonSelectedObjectClearClick() 
{
	glComponent->clearLastSelectedObject();
	labelSelectedObject->clear();
}

void MainForm_TextureSelectionComponent::onLabelImageTextureMouseReleaseEvent(QMouseEvent *event) 
{
	int comboBoxTextureCurrentIndex { comboBoxTexture->currentIndex() };
	string textureName { comboBoxTexture->itemText(comboBoxTextureCurrentIndex).toStdString() };
	auto texture { glComponent->getTextureByName(textureName) };
	if (texture.has_value()) {
		string name { textureName };
		int index = getTextureIndexFromPosition(Point(event->pos().x(), event->pos().y()), texture.get());
		//Display the selected texture or object on the selected image
		auto imagePart { getTextureTileImageFromTexture(index, texture.get()) };
        auto selectionMode { glComponent->getSelectionMode() };
		if (selectionMode == SelectionMode::ApplyTexture) {
			glComponent->setLastSelectedTexture(name, index);
			labelSelectedTexture->setPixmap(imagePart);
		}
		else if (selectionMode == SelectionMode::ApplyObject) {
			glComponent->setLastSelectedObject(name, index);
			labelSelectedObject->setPixmap(imagePart);
		}
	}	
}

void MainForm_TextureSelectionComponent::onComboBoxTextureCurrentIndexChanged() 
{
	displaySelectedTextureImage();
}