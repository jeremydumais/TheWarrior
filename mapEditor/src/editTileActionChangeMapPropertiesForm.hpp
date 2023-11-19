#ifndef MAPEDITOR_SRC_EDITTILEACTIONCHANGEMAPPROPERTIESFORM_HPP_
#define MAPEDITOR_SRC_EDITTILEACTIONCHANGEMAPPROPERTIESFORM_HPP_

#include "ui_editTileActionChangeMapPropertiesForm.h"
#include <map>
#include <string>

class EditTileActionChangeMapPropertiesForm : public QDialog {
Q_OBJECT

 public:
    explicit EditTileActionChangeMapPropertiesForm(QWidget *parent,
            const std::string &resourcesPath,
            const std::map<std::string, std::string> &properties);
    ~EditTileActionChangeMapPropertiesForm() override = default;
    const std::map<std::string, std::string> &getUpdatedProperties() const;
 private:
    Ui::editTileActionChangeMapPropertiesFormClass ui;
    std::string m_resourcesPath;
    std::map<std::string, std::string> m_properties;
    void onPushButtonOK();
    void onPushButtonOpenMapFileClick();
};

#endif  // MAPEDITOR_SRC_EDITTILEACTIONCHANGEMAPPROPERTIESFORM_HPP_
