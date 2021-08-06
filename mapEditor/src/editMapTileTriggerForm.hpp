
#ifndef EDITMAPTILETRIGGERFORM_H
#define EDITMAPTILETRIGGERFORM_H

#include "ui_editMapTileTriggerForm.h"

class EditMapTileTriggerForm : public QDialog
{
Q_OBJECT

public:
	explicit EditMapTileTriggerForm(QWidget *parent);
	~EditMapTileTriggerForm() = default;
private:
	Ui::editMapTileTriggerFormClass ui;
	bool isEditMode;
	void showErrorMessage(const std::string &message) const;
	void onPushButtonOK();
};

#endif // EDITMAPTILETRIGGERFORM_H
