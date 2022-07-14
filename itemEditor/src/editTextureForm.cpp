#include "editTextureForm.hpp"
#include "errorMessage.hpp"
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <fmt/format.h>

EditTextureForm::EditTextureForm(QWidget *parent,
								 const std::string &resourcesPath, 
								 std::unique_ptr<TextureDTO> originalTexture,
								 const std::vector<std::string> &allTextureNames)
	: QDialog(parent),
	  ui(Ui::editTextureFormClass()),
	  m_isEditMode(originalTexture != nullptr),
	  m_controller(EditTextureFormController(std::move(originalTexture), allTextureNames)),
	  m_resourcesPath(resourcesPath){
	ui.setupUi(this);
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditTextureForm::onPushButtonOK);
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditTextureForm::reject);
	connect(ui.pushButtonOpenFilename, &QPushButton::clicked, this, &EditTextureForm::onPushButtonOpenFilenameClick);

	if (!m_isEditMode) {
		this->setWindowTitle("Add texture");
	}
	else {
		this->setWindowTitle("Edit texture");
		loadExistingItemToForm();
	}
}

std::unique_ptr<TextureDTO> EditTextureForm::getTextureInfo() const
{
	return createTextureDTOFromFields();
}

void EditTextureForm::loadExistingItemToForm()
{
	const auto &texture = m_controller.getOriginalTexture();
	ui.lineEditName->setText(texture.name.c_str());
	ui.lineEditFilename->setText(texture.filename.c_str());
	ui.spinBoxWidth->setValue(texture.width);
	ui.spinBoxHeight->setValue(texture.height);
	ui.spinBoxTileWidth->setValue(texture.tileWidth);
	ui.spinBoxTileHeight->setValue(texture.tileHeight);
	QImage image;
	std::string imageFullPath { fmt::format("{0}/textures/{1}", m_resourcesPath, texture.filename) };
	if (!image.load(imageFullPath.c_str())) {
		ErrorMessage::show(fmt::format("Unable to load the image {0}", imageFullPath));
		ui.lineEditFilename->clear();
	} 
	else {
		ui.labelImage->setFixedSize(image.width(), image.height());
		ui.labelImage->setPixmap(QPixmap::fromImage(image));
	}
}

void EditTextureForm::onPushButtonOK() 
{
	auto textureInfo = createTextureDTOFromFields();

	if (!m_controller.validateTextureOperation(std::move(textureInfo))) {
		ErrorMessage::show(m_controller.getLastError());
		return;
	}
	accept();
}

void EditTextureForm::onPushButtonOpenFilenameClick() 
{
	QString fullFilePath { QFileDialog::getOpenFileName(this, 
														tr("Open Texture"), 
														m_resourcesPath.c_str(), 
														tr("Images (*.png)")) };
	QFileInfo fileInfo(fullFilePath);
	std::string filename { fileInfo.fileName().toStdString() };
	ui.lineEditFilename->setText(filename.c_str());
	//Detect width and height of the image
	QImage image;
	
	std::string imageFullPath { fmt::format("{0}/textures/{1}", m_resourcesPath, filename) };
	if (!image.load(imageFullPath.c_str())) {
		ErrorMessage::show(fmt::format("Unable to load the image {0}", imageFullPath));
		ui.lineEditFilename->clear();
	}
	else {
		ui.labelImage->setFixedSize(image.width(), image.height());
		ui.labelImage->setPixmap(QPixmap::fromImage(image));
		ui.spinBoxWidth->setValue(image.width());
		ui.spinBoxHeight->setValue(image.height());
	}
}

std::unique_ptr<TextureDTO> EditTextureForm::createTextureDTOFromFields() const
{
	return std::unique_ptr<TextureDTO>(new TextureDTO {
		ui.lineEditName->text().toStdString(),
		ui.lineEditFilename->text().toStdString(),
		ui.spinBoxWidth->value(),
		ui.spinBoxHeight->value(),
		ui.spinBoxTileWidth->value(),
		ui.spinBoxTileHeight->value()
	});
}
