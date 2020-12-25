
#ifndef EDITTEXTUREFORM_H
#define EDITTEXTUREFORM_H

#include "texture.hpp"
#include "ui_editTextureForm.h"
#include <string>

class EditTextureForm : public QDialog
{
Q_OBJECT

public:
	explicit EditTextureForm(QWidget *parent, 
							 const std::string &executablePath, 
							 const Texture * const texture);
	~EditTextureForm() = default;
private:
	Ui::editTextureFormClass ui;
	std::string executablePath;
	const Texture * const texture;
};

#endif // EDITTEXTUREFORM_H
