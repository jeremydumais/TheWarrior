#ifndef EDITTILEACTIONCHANGEMAPPROPERTIESFORM_H
#define EDITTILEACTIONCHANGEMAPPROPERTIESFORM_H

#include "ui_editTileActionChangeMapPropertiesForm.h"
#include <map>
#include <string>

class EditTileActionChangeMapPropertiesForm : public QDialog
{
Q_OBJECT

public:
	explicit EditTileActionChangeMapPropertiesForm(QWidget *parent, 
							 const std::string &resourcesPath,
							 const std::map<std::string, std::string> &properties);
	~EditTileActionChangeMapPropertiesForm() = default;
	const std::map<std::string, std::string> &getUpdatedProperties() const;
private:
	Ui::editTileActionChangeMapPropertiesFormClass ui;
	std::string resourcesPath;
	std::map<std::string, std::string> properties;
	void showErrorMessage(const std::string &message) const;
	void onPushButtonOK();
	void onPushButtonOpenMapFileClick();
};

#endif // EDITTILEACTIONCHANGEMAPPROPERTIESFORM_H
