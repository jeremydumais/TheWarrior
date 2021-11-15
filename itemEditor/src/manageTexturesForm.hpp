#ifndef MANAGETEXTURESFORM_H
#define MANAGETEXTURESFORM_H

#include "manageTexturesController.hpp"
#include "ui_manageTexturesForm.h"
#include <string>

class ManageTexturesForm : public QDialog
{
Q_OBJECT

public:
	ManageTexturesForm(QWidget *parent, 
					   const std::string &resourcesPath,
					   TextureContainer &textureContainer);
	void connectUIActions();
private:
	Ui::manageTexturesFormClass ui;
	const std::string m_resourcesPath;
	ManageTextureController m_controller;
	void refreshTextureList();
	void onPushButtonCloseClick();
	void onPushButtonAddClick();
};

#endif // MANAGETEXTURESFORM_H
