
#ifndef MAPEDITOR_SRC_EDITMAPTILETRIGGERFORM_HPP_
#define MAPEDITOR_SRC_EDITMAPTILETRIGGERFORM_HPP_

#include <string>
#include <vector>
#include <boost/optional/optional.hpp>
#include "editMapTileTriggerFormController.hpp"
#include "mapTileTriggerDTO.hpp"
#include "ui_editMapTileTriggerForm.h"

class EditMapTileTriggerForm : public QDialog {
    Q_OBJECT

 public:
    explicit EditMapTileTriggerForm(QWidget *parent,
            const std::string &resourcesPath,
            const boost::optional<mapeditor::controllers::MapTileTriggerDTO> currentTrigger,
            const std::vector<mapeditor::controllers::MapTileTriggerDTO> &allTriggers);
    ~EditMapTileTriggerForm() override = default;
    const mapeditor::controllers::MapTileTriggerDTO &getUpdatedTrigger() const;

 private:
    Ui::editMapTileTriggerFormClass ui;
    mapeditor::controllers::EditMapTileTriggerFormController m_controller;
    std::string m_resourcesPath;
    bool m_isEditMode;
    void generateComboxItems();
    void refreshComboBoxEvent();
    void refreshComboBoxCondition();
    void refreshComboBoxAction();
    void onPushButtonTileActionPropertiesClick();
    void onPushButtonOK();
    void onComboBoxEventIndexChanged();
    void onComboBoxConditionIndexChanged();
    void onComboBoxActionIndexChanged();
};

#endif  // MAPEDITOR_SRC_EDITMAPTILETRIGGERFORM_HPP_
