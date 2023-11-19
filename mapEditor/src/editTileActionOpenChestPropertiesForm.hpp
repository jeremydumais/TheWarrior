#ifndef MAPEDITOR_SRC_EDITTILEACTIONOPENCHESTPROPERTIESFORM_HPP_
#define MAPEDITOR_SRC_EDITTILEACTIONOPENCHESTPROPERTIESFORM_HPP_

#include "ui_editTileActionOpenChestPropertiesForm.h"
#include <map>
#include <string>

class EditTileActionOpenChestPropertiesForm : public QDialog {
Q_OBJECT

 public:
    explicit EditTileActionOpenChestPropertiesForm(QWidget *parent,
            const std::map<std::string, std::string> &properties);
    ~EditTileActionOpenChestPropertiesForm() override = default;
    const std::map<std::string, std::string> &getUpdatedProperties() const;

 private:
    Ui::editTileActionOpenChestPropertiesFormClass ui;
    std::map<std::string, std::string> m_properties;
    void showErrorMessage(const std::string &message) const;
    void onPushButtonOK();
};

#endif  // MAPEDITOR_SRC_EDITTILEACTIONOPENCHESTPROPERTIESFORM_HPP_
