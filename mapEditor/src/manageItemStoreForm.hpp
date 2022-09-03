
#ifndef MANAGEITEMSTOREFORM_H
#define MANAGEITEMSTOREFORM_H

#include "ui_manageItemStoreForm.h"
#include "itemStoreInfo.hpp"
#include "manageItemStoreController.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include <qdialog.h>
#include <optional>
#include <string>
#include <vector>

class ManageItemStoreForm : public QDialog
{
Q_OBJECT

public:
    explicit ManageItemStoreForm(QWidget *parent,
                                 const std::string &resourcesPath,
                                 const std::string &userConfigFolder);
private:
    Ui::manageItemStoreFormClass ui;
    mapeditor::controllers::ManageItemStoreController m_controller;
    QTableWidgetKeyPressWatcher tableWidgetItemKeyWatcher;
    void connectUIActions();
    void initializeItemTable();
    void refreshItemStoreList();
    void onPushButtonAddClick();
    void onPushButtonEditClick();
    void onPushButtonDeleteClick();
    void onTableWidgetItemKeyPressEvent(int key, int, int);
    std::optional<std::string> getSelectedItemStoreName() const;
};

#endif // MANAGEITEMSTOREFORM_H
