
#ifndef MANAGEITEMSTOREFORM_H
#define MANAGEITEMSTOREFORM_H

#include "ui_manageItemStoreForm.h"
#include "itemStoreInfo.hpp"
#include "manageItemStoreController.hpp"
#include <qdialog.h>
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
    void connectUIActions();
    void initializeItemTable();
    void refreshItemStoreList();
    void onPushButtonAddClick();
    void onPushButtonEditClick();
    void onPushButtonDeleteClick();
};

#endif // MANAGEITEMSTOREFORM_H
