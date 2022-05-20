
#ifndef EDITTEXTUREFORM_H
#define EDITTEXTUREFORM_H

#include "editTextureFormController.hpp"
#include "textureDTO.hpp"
#include "ui_editTextureForm.h"
#include <memory>
#include <string>
#include <vector>

class EditTextureForm : public QDialog
{
Q_OBJECT

public:
	explicit EditTextureForm(QWidget *parent, 
							 const std::string &resourcesPath, 
							 std::unique_ptr<TextureDTO> originalTexture,
							 const std::vector<std::string> &allTextureNames);
	std::unique_ptr<TextureDTO> getTextureInfo() const;
private:
	Ui::editTextureFormClass ui;
	bool m_isEditMode;
	EditTextureFormController m_controller;
	std::string m_resourcesPath;
	void loadExistingItemToForm();
	void onPushButtonOK();
	void onPushButtonOpenFilenameClick();
	std::unique_ptr<TextureDTO> createTextureDTOFromFields() const;
};

#endif // EDITTEXTUREFORM_H
