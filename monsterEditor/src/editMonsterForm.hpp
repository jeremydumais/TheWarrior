#ifndef EDITMONSTERFORM_H
#define EDITMONSTERFORM_H

#include "ui_editMonsterForm.h"
#include "manageMonsterController.hpp"
#include "monsterStore.hpp"
#include <memory>
#include <optional>
#include <string>

class EditMonsterForm : public QDialog
{
Q_OBJECT

public:
	EditMonsterForm(QWidget *parent, 
				const std::string &resourcesPath,
				std::shared_ptr<MonsterStore> monsterStore,
				std::optional<std::string> monsterIdToEdit);
	void connectUIActions();
	
private:
	Ui::editMonsterFormClass ui;
	ManageMonsterController m_controller;
    std::string m_resourcesPath;
    std::optional<std::string> m_monsterIdToEdit;
	bool loadExistingMonsterToForm();
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void onPushButtonTexturePickerClick();
    void refreshSelectedTexture();
};

#endif // EDITMONSTERFORM_H
