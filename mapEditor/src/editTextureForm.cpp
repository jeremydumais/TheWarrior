#include "editTextureForm.hpp"
#include <fmt/format.h>
#include <QImageReader>

using namespace std;

EditTextureForm::EditTextureForm(QWidget *parent,
								 const std::string &executablePath, 
								 const Texture * const texture)
	: QDialog(parent),
	  ui(Ui::editTextureFormClass()),
	  executablePath(executablePath),
	  texture(texture)
{
	ui.setupUi(this);
	connect(ui.pushButtonClose, &QPushButton::clicked, this, &EditTextureForm::close);
	QImageReader reader(fmt::format("{0}/resources/{1}", executablePath, texture->getFilename()).c_str());
	const QImage image = reader.read();
	ui.labelImage->setFixedSize(image.width(), image.height());
	//this->setFixedSize(image.width() + 20, image.height() + 80);
	ui.labelImage->setPixmap(QPixmap::fromImage(image));

}