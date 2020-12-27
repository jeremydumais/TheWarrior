#include "editTextureForm.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>

using namespace std;

EditTextureForm::EditTextureForm(QWidget *parent,
								 const std::string &resourcesPath, 
								 const Texture * const texture,
								 const std::vector<std::string> &alreadyUsedNames)
	: QDialog(parent),
	  ui(Ui::editTextureFormClass()),
	  resourcesPath(resourcesPath),
	  alreadyUsedNames(alreadyUsedNames),
	  isEditMode(texture != nullptr)
{
	ui.setupUi(this);
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditTextureForm::onPushButtonOK);
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditTextureForm::reject);
	connect(ui.pushButtonOpenFilename, &QPushButton::clicked, this, &EditTextureForm::onPushButtonOpenFilenameClick);

	if (!isEditMode) {
		this->setWindowTitle("Add texture");
	}
	else {
		this->setWindowTitle("Edit texture");
		ui.lineEditName->setText(texture->getName().c_str());
		ui.lineEditFilename->setText(texture->getFilename().c_str());
		ui.spinBoxWidth->setValue(texture->getWidth());
		ui.spinBoxHeight->setValue(texture->getHeight());
		ui.spinBoxTileWidth->setValue(texture->getTileWidth());
		ui.spinBoxTileHeight->setValue(texture->getTileHeight());
		QImage image;
		string imageFullPath { fmt::format("{0}{1}", resourcesPath, texture->getFilename()) };
		if (!image.load(imageFullPath.c_str())) {
			showErrorMessage(fmt::format("Unable to load the image {0}", imageFullPath));
			ui.lineEditFilename->clear();
		} 
		else {
			ui.labelImage->setFixedSize(image.width(), image.height());
			ui.labelImage->setPixmap(QPixmap::fromImage(image));
		}
	}
}

const TextureInfo& EditTextureForm::getTextureInfo() const
{
	return textureInfo;
}

void EditTextureForm::onPushButtonOK() 
{
	textureInfo.name = ui.lineEditName->text().toStdString();
	textureInfo.filename = ui.lineEditFilename->text().toStdString();
	textureInfo.width = ui.spinBoxWidth->value();
	textureInfo.height = ui.spinBoxHeight->value();
	textureInfo.tileWidth = ui.spinBoxTileWidth->value();
	textureInfo.tileHeight = ui.spinBoxTileHeight->value();

	try {
		auto texture = make_shared<Texture>(textureInfo);
	}
	catch(invalid_argument &err) {
		showErrorMessage(err.what());
		return;
	}
	//Check if the texture name is available
	if (std::find(alreadyUsedNames.begin(), alreadyUsedNames.end(), textureInfo.name) != alreadyUsedNames.end()) {
		showErrorMessage(fmt::format("The name {0} is already used.", textureInfo.name));
		return;
	}
	accept();
}

void EditTextureForm::showErrorMessage(const std::string &message) const
{
	QMessageBox msgBox;
	msgBox.setText(message.c_str());
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setWindowTitle("Error");
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();
}

void EditTextureForm::onPushButtonOpenFilenameClick() 
{
	QString fullFilePath { QFileDialog::getOpenFileName(this, 
														tr("Open Texture"), 
														resourcesPath.c_str(), 
														tr("Images (*.png)")) };
	QFileInfo fileInfo(fullFilePath);
	string filename { fileInfo.fileName().toStdString() };
	ui.lineEditFilename->setText(filename.c_str());
	//Detect width and height of the image
	QImage image;
	string imageFullPath { fmt::format("{0}{1}", resourcesPath, filename) };
	if (!image.load(imageFullPath.c_str())) {
		showErrorMessage(fmt::format("Unable to load the image {0}", imageFullPath));
		ui.lineEditFilename->clear();
	}
	else {
		ui.labelImage->setFixedSize(image.width(), image.height());
		ui.labelImage->setPixmap(QPixmap::fromImage(image));
		ui.spinBoxWidth->setValue(image.width());
		ui.spinBoxHeight->setValue(image.height());
	}
}