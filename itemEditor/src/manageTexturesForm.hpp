#ifndef MANAGETEXTURESFORM_H
#define MANAGETEXTURESFORM_H

#include "manageTexturesController.hpp"
#include "ui_manageTexturesForm.h"
#include <optional>
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
	std::optional<std::reference_wrapper<const Texture>> getSelectedTextureInTextureList();
	void onPushButtonAddClick();
	void onPushButtonEditClick();
};

#endif // MANAGETEXTURESFORM_H
