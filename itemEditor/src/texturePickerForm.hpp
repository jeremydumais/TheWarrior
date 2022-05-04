#ifndef TEXTUREPICKERFORM_H
#define TEXTUREPICKERFORM_H

#include "ui_texturePickerForm.h"
#include "textureContainer.hpp"
#include "texturePickerController.hpp"
#include <string>

struct PickerResult 
{
	std::string textureName;
	int textureIndex;
};

class TexturePickerForm : public QDialog
{
Q_OBJECT

public:
	TexturePickerForm(QWidget *parent, 
				      const std::string &resourcesPath,
					  const TextureContainer &textureContainer);
	void connectUIActions();
	PickerResult getResult() const;
private:
	Ui::texturePickerFormClass ui;
	const std::string &m_resourcesPath;
	TexturePickerController m_controller;
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void refreshTextureComboBox();
	void onLabelImageTextureMouseReleaseEvent(QMouseEvent *event);
	void onComboBoxTextureCurrentIndexChanged();

};

#endif // TEXTUREPICKERFORM_H
