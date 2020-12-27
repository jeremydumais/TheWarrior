
#ifndef EDITTEXTUREFORM_H
#define EDITTEXTUREFORM_H

#include "texture.hpp"
#include "ui_editTextureForm.h"
#include <vector>
#include <string>

class EditTextureForm : public QDialog
{
Q_OBJECT

public:
	explicit EditTextureForm(QWidget *parent, 
							 const std::string &resourcesPath, 
							 const Texture * const texture,
							 const std::vector<std::string> &alreadyUsedNames);
	~EditTextureForm() = default;
	const TextureInfo &getTextureInfo() const;
private:
	Ui::editTextureFormClass ui;
	std::string resourcesPath;
	TextureInfo textureInfo;
	const std::vector<std::string> &alreadyUsedNames;
	bool isEditMode;
	void showErrorMessage(const std::string &message) const;
	void onPushButtonOK();
	void onPushButtonOpenFilenameClick();
};

#endif // EDITTEXTUREFORM_H
