
#ifndef EDITMAPTILETRIGGERFORM_H
#define EDITMAPTILETRIGGERFORM_H

#include "ui_editMapTileTriggerForm.h"
#include "mapTileTrigger.hpp"

class EditMapTileTriggerForm : public QDialog
{
Q_OBJECT

public:
	explicit EditMapTileTriggerForm(QWidget *parent,
									const std::string &resourcesPath,
									const thewarrior::models::MapTileTrigger * const currentTrigger,
									const std::vector<thewarrior::models::MapTileTrigger> &allTriggers);
	~EditMapTileTriggerForm() = default;
	const thewarrior::models::MapTileTrigger &getUpdatedTrigger() const;
private:
	Ui::editMapTileTriggerFormClass ui;
	std::string m_resourcesPath;
	const thewarrior::models::MapTileTrigger * const m_currentTrigger;
	const std::vector<thewarrior::models::MapTileTrigger> &m_allTriggers;
	thewarrior::models::MapTileTrigger m_updatedTrigger;
	bool m_isEditMode;
	void generateComboxItems();
	void refreshComboBoxEvent();
    void refreshComboBoxCondition();
    void refreshComboBoxAction();
	void onPushButtonTileActionPropertiesClick();
	void onPushButtonOK();

};

#endif // EDITMAPTILETRIGGERFORM_H
