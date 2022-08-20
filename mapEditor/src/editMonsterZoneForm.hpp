
#ifndef EDITMONSTERZONEFORM_H
#define EDITMONSTERZONEFORM_H

#include "ui_editMonsterZoneForm.h"
#include <string>
#include <vector>

class EditMonsterZoneForm : public QDialog
{
Q_OBJECT

public:
    explicit EditMonsterZoneForm(QWidget *parent);
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
