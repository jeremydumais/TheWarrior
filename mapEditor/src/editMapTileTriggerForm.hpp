
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
									const MapTileTrigger * const currentTrigger,
									const std::vector<MapTileTrigger> &allTriggers);
	~EditMapTileTriggerForm() = default;
	const MapTileTrigger &getUpdatedTrigger() const;
private:
	Ui::editMapTileTriggerFormClass ui;
	std::string m_resourcesPath;
	const MapTileTrigger * const m_currentTrigger;
	const std::vector<MapTileTrigger> &m_allTriggers;
	MapTileTrigger m_updatedTrigger;
	bool m_isEditMode;
	void generateComboxItems();
	void refreshComboBoxEvent();
    void refreshComboBoxCondition();
    void refreshComboBoxAction();
	void onPushButtonTileActionPropertiesClick();
	void onPushButtonOK();
	
};

#endif // EDITMAPTILETRIGGERFORM_H
