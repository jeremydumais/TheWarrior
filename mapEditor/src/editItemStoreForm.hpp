
#ifndef EDITITEMSTOREFORM_H
#define EDITITEMSTOREFORM_H

#include "ui_editItemStoreForm.h"
#include "editItemStoreController.hpp"
#include "itemStoreInfo.hpp"
#include <qdialog.h>
#include <string>
#include <vector>

class EditItemStoreForm : public QDialog
{
Q_OBJECT

public:
    explicit EditItemStoreForm(QWidget *parent,
                               const std::string &resourcesPath,
                               const std::string &userConfigFolder,
                               const std::vector<mapeditor::controllers::ItemStoreInfo> &itemStores);
private:
    Ui::editItemStoreFormClass ui;
    mapeditor::controllers::EditItemStoreController m_controller;
    void connectUIActions();
    void onPushButtonOKClick();
    void onPushButtonOpenFileClick();
};

#endif // EDITITEMSTOREFORM_H
