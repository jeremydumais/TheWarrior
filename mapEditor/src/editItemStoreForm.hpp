
#ifndef EDITITEMSTOREFORM_H
#define EDITITEMSTOREFORM_H

#include "ui_editItemStoreForm.h"
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
                               const std::vector<ItemStoreInfo> &itemStores);
private:
    Ui::editItemStoreFormClass ui;
    std::string m_resourcesPath = "";
    std::string m_userConfigFolder = "";
    std::vector<ItemStoreInfo> m_itemStores;
    void connectUIActions();
    void onPushButtonOKClick();
    void onPushButtonOpenFileClick();
};

#endif // EDITITEMSTOREFORM_H
