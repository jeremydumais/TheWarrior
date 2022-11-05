
#ifndef EDITMONSTERZONEFORM_H
#define EDITMONSTERZONEFORM_H

#include "ui_editMonsterZoneForm.h"
#include "editMonsterZoneFormController.hpp"
#include "manageMonsterStoreController.hpp"
#include "types.hpp"
#include <memory>
#include <string>
#include <vector>

class EditMonsterZoneForm : public QDialog
{
Q_OBJECT

public:
    explicit EditMonsterZoneForm(QWidget *parent,
                                 const std::shared_ptr<mapeditor::controllers::VecOfMonsterStore> monsterStores);
private:
    struct ColorItem
    {
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
    std::shared_ptr<mapeditor::controllers::VecOfMonsterStore> m_monsterStores;
    void connectUIActions();
    void initializeColors();
    void initializeMonsterTable();
    void onPushButtonOK();
    void onComboBoxColorCurrentIndexChanged();
    void onPushButtonAddMonsterClick();
    void onPushButtonEditMonsterClick();
    void onPushButtonDeleteMonsterClick();
};

#endif // EDITMONSTERZONEFORM_H
