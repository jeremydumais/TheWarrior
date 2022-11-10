#ifndef EDITMONSTERENCOUNTERFORM_H
#define EDITMONSTERENCOUNTERFORM_H

#include "ui_editMonsterEncounterForm.h"
#include "editMonsterEncounterFormController.hpp"
#include "monsterEncounterDTO.hpp"
#include "types.hpp"
#include <memory>
#include <string>
#include <vector>

class EditMonsterEncounterForm : public QDialog
{
Q_OBJECT

public:
    explicit EditMonsterEncounterForm(QWidget *parent,
                                      const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores,
                                      const std::string &resourcesPath,
                                      const std::vector<std::string> &alreadyUsedMonsterIds,
                                      const std::optional<mapeditor::controllers::MonsterEncounterDTO> &editMonsterInfo);
    mapeditor::controllers::MonsterEncounterDTO getResult() const;
private:
    Ui::editMonsterEncounterFormClass ui;
    mapeditor::controllers::EditMonsterEncounterFormController m_controller;
    std::optional<mapeditor::controllers::MonsterEncounterDTO> m_editMonsterInfo;
    void connectUIActions();
    void populateComboBoxEncounter();
    bool loadExistingItemToForm();
    void onPushButtonOKClick();
    void onPushButtonMonsterSelectorClick();
    void onLineEditIdEditingFinish();
};

#endif // EDITMONSTERENCOUNTERFORM_H
