
#ifndef MONSTERPICKERFORM_H
#define MONSTERPICKERFORM_H

#include "ui_monsterPickerForm.h"
#include "monsterPickerFormController.hpp"
#include "types.hpp"
#include <memory>
#include <string>
#include <vector>

class MonsterPickerForm : public QDialog
{
Q_OBJECT

public:
    explicit MonsterPickerForm(QWidget *parent,
                               const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores);
private:
    Ui::monsterPickerFormClass ui;
    mapeditor::controllers::MonsterPickerFormController m_controller;
    void connectUIActions();
    void populateMonsterStoreComboBox();
    void initializeMonsterTable();
    void refreshMonsterTable(const std::string &filter = "");
    void onLineEditSearchTextChanged(const QString &text);
    void onComboBoxStoreIndexChanged();
};

#endif // MONSTERPICKERFORM_H
