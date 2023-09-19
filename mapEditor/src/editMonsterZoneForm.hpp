
#ifndef MAPEDITOR_SRC_EDITMONSTERZONEFORM_HPP_
#define MAPEDITOR_SRC_EDITMONSTERZONEFORM_HPP_

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "ui_editMonsterZoneForm.h"
#include "editMonsterZoneFormController.hpp"
#include "manageMonsterStoreController.hpp"
#include "monsterZoneDTO.hpp"
#include "types.hpp"

class EditMonsterZoneForm : public QDialog {
Q_OBJECT

 public:
    EditMonsterZoneForm(QWidget *parent,
                        const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores,
                        const std::string &resourcesPath,
                        const std::optional<mapeditor::controllers::MonsterZoneDTO> selectedZone,
                        const std::vector<std::string> &alreadyUsedZoneNames);
    const mapeditor::controllers::MonsterZoneDTO &getResult() const;

 private:
    struct ColorItem {
        std::string displayName;
        std::string value;
    };
    Ui::editMonsterZoneFormClass ui;
    std::vector<ColorItem> colors {
        { "Green", "#00FF00" },
        { "Red", "#FF0000" },
        { "Blue", "#0000FF" },
        { "Yellow", "#e8ff1d" },
        { "Pink", "#ff3dfc"},
        { "Cyan", "#49fff3" },
        { "Orange", "#ff711e" },
        { "Purple", "#c8a4ff" }
    };
    mapeditor::controllers::EditMonsterZoneFormController m_controller;
    mapeditor::controllers::MonsterZoneDTO m_result;
    std::vector<std::string> m_alreadyUsedZoneNames;
    void connectUIActions();
    void initializeColors();
    void initializeMonsterTable();
    void refreshMonsterEncounterList();
    void onPushButtonOK();
    void onComboBoxColorCurrentIndexChanged();
    void onPushButtonAddMonsterClick();
    void onPushButtonEditMonsterClick();
    void onPushButtonDeleteMonsterClick();
    std::optional<std::string> getSelectedItemId() const;
    std::vector<EditMonsterZoneForm::ColorItem>::difference_type findColorIndexByName(std::string_view name) const;
};

#endif  // MAPEDITOR_SRC_EDITMONSTERZONEFORM_HPP_
