#ifndef MAPEDITOR_SRC_MONSTERPICKERFORM_HPP_
#define MAPEDITOR_SRC_MONSTERPICKERFORM_HPP_

#include <memory>
#include <string>
#include "ui_monsterPickerForm.h"
#include "monsterPickerFormController.hpp"
#include "types.hpp"

class MonsterPickerForm : public QDialog{
Q_OBJECT

 public:
    explicit MonsterPickerForm(QWidget *parent,
                               const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores,
                               const std::string &resourcesPath);
    const std::string &getSelectedMonsterId() const;

 private:
    Ui::monsterPickerFormClass ui;
    mapeditor::controllers::MonsterPickerFormController m_controller;
    std::string m_selectedMonsterId = "";
    void connectUIActions();
    void populateMonsterStoreComboBox();
    void initializeMonsterTable();
    void refreshMonsterTable(const std::string &filter = "");
    void onPushButtonOkClicked();
    void onLineEditSearchTextChanged(const QString &text);
    void onComboBoxStoreIndexChanged();
};

#endif  // MAPEDITOR_SRC_MONSTERPICKERFORM_HPP_
