
#ifndef MANAGEMONSTERSTOREFORM_H
#define MANAGEMONSTERSTOREFORM_H

#include "ui_manageMonsterStoreForm.h"
#include "itemStoreInfo.hpp"
#include "manageMonsterStoreController.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include <qdialog.h>
#include <optional>
#include <string>
#include <vector>

class ManageMonsterStoreForm : public QDialog
{
Q_OBJECT

public:
    explicit ManageMonsterStoreForm(QWidget *parent,
                                 const std::string &resourcesPath,
                                 const std::string &userConfigFolder);
private:
    Ui::manageMonsterStoreFormClass ui;
    mapeditor::controllers::ManageMonsterStoreController m_controller;
    QTableWidgetKeyPressWatcher tableWidgetMonsterKeyWatcher;
    void connectUIActions();
    void initializeMonsterTable();
    void refreshMonsterStoreList();
    void onPushButtonAddClick();
    void onPushButtonEditClick();
    void onPushButtonDeleteClick();
    void onTableWidgetMonsterKeyPressEvent(int key, int, int);
    std::optional<std::string> getSelectedMonsterStoreName() const;
};

#endif // MANAGEMONSTERSTOREFORM_H
