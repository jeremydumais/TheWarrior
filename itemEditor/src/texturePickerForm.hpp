#ifndef TEXTUREPICKERFORM_H
#define TEXTUREPICKERFORM_H

#include "ui_texturePickerForm.h"
#include "textureContainer.hpp"
#include "texturePickerController.hpp"
#include <string>

class TexturePickerForm : public QDialog
{
Q_OBJECT

public:
	TexturePickerForm(QWidget *parent, 
				      const std::string &resourcesPath,
					  const TextureContainer &textureContainer);
	void connectUIActions();
private:
	Ui::texturePickerFormClass ui;
	const std::string &m_resourcesPath;
	TexturePickerController m_controller;
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void refreshTextureComboBox();
	void onLabelImageTextureMouseReleaseEvent(QMouseEvent *event);
	void onComboBoxTextureCurrentIndexChanged();
	void displaySelectedTextureImage();

};

#endif // TEXTUREPICKERFORM_H
