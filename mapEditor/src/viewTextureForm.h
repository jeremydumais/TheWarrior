
#ifndef VIEWTEXTUREFORM_H
#define VIEWTEXTUREFORM_H

#include "texture.hpp"
#include "ui_viewTextureForm.h"
#include <string>

class ViewTextureForm : public QDialog
{
Q_OBJECT

public:
	explicit ViewTextureForm(QWidget *parent, 
							 const std::string &executablePath, 
							 const Texture * const texture);
	~ViewTextureForm() = default;
private:
	Ui::viewTextureFormClass ui;
	std::string executablePath;
	const Texture * const texture;
};

#endif // VIEWTEXTUREFORM_H
