#include "texturePickerForm.hpp"
#include "qClickableLabel.hpp"
#include <fmt/format.h>
#include <qimage.h>
#include <qimagereader.h>
#include <qpixmap.h>
#include <string>

TexturePickerForm::TexturePickerForm(QWidget *parent, 
				    				 const std::string &resourcesPath,
									 const TextureContainer &textureContainer)
	: QDialog(parent),
	  ui(Ui::texturePickerFormClass()),
	  m_resourcesPath(resourcesPath),
	  m_controller(textureContainer)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
	connectUIActions();
	refreshTextureComboBox();
}

void TexturePickerForm::refreshTextureComboBox()
{
	ui.comboBoxTexture->model()->removeRows(0, ui.comboBoxTexture->count());
	int i = 0;
	for (const auto &texture : m_controller.getTextures()) {
		ui.comboBoxTexture->insertItem(i, texture.getName().c_str());
		i++;
	}
}

void TexturePickerForm::connectUIActions() 
{
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &TexturePickerForm::onPushButtonCancelClick);
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &TexturePickerForm::onPushButtonOKClick);
	connect(ui.labelImageTexture, &QClickableLabel::onMouseReleaseEvent, this, &TexturePickerForm::onLabelImageTextureMouseReleaseEvent);
	connect(ui.comboBoxTexture, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &TexturePickerForm::onComboBoxTextureCurrentIndexChanged);

}

void TexturePickerForm::onPushButtonCancelClick()
{
	close();
}

void TexturePickerForm::onPushButtonOKClick()
{
}

void TexturePickerForm::onLabelImageTextureMouseReleaseEvent(QMouseEvent *event) 
{
	int comboBoxTextureCurrentIndex { ui.comboBoxTexture->currentIndex() };
	std::string textureName { ui.comboBoxTexture->itemText(comboBoxTextureCurrentIndex).toStdString() };
	auto texture { m_controller.getTextureByName(textureName) };
	if (texture.has_value()) {
		std::string name { textureName };
		//TODO Put the code of getTextureIndexFromPosition in the qClickableLabelClass
		//int index = m_controller.getTextureIndexFromPosition(Point(event->pos().x(), event->pos().y()), texture->get());
		//Display the selected texture or object on the selected image
		/*auto imagePart { getTextureTileImageFromTexture(index, texture->get()) };
        auto selectionMode { m_glComponent->getSelectionMode() };
		if (selectionMode == SelectionMode::ApplyTexture) {
			m_glComponent->setLastSelectedTexture(name, index);
			m_labelSelectedTexture->setPixmap(imagePart);
		}
		else if (selectionMode == SelectionMode::ApplyObject) {
			m_glComponent->setLastSelectedObject(name, index);
			m_labelSelectedObject->setPixmap(imagePart);
		}*/
	}	
}

void TexturePickerForm::onComboBoxTextureCurrentIndexChanged() 
{
	displaySelectedTextureImage();
}

void TexturePickerForm::displaySelectedTextureImage() 
{
	//Find the selected texture
	auto texture { m_controller.getTextureByName(ui.comboBoxTexture->itemText(ui.comboBoxTexture->currentIndex()).toStdString()) };
	if (texture.has_value()) {
		QImageReader reader(fmt::format("{0}/{1}", m_resourcesPath, texture->get().getFilename()).c_str());
		const QImage image { reader.read() };
		ui.labelImageTexture->setFixedSize(image.width(), image.height());
		ui.labelImageTexture->setPixmap(QPixmap::fromImage(image));
		ui.lineEditTextureName->setText(texture->get().getName().c_str());
	}
	else {
		ui.labelImageTexture->clear();
		ui.lineEditTextureName->clear();
		ui.spinBoxTextureIndex->setValue(0);
	}
}