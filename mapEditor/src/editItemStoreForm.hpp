
#ifndef EDITITEMSTOREFORM_H
#define EDITITEMSTOREFORM_H

#include "ui_editItemStoreForm.h"
#include "manageItemStoreController.hpp"
#include "itemStoreInfo.hpp"
#include <functional>
#include <optional>
#include <qdialog.h>
#include <string>
#include <vector>

class EditItemStoreForm : public QDialog
{
Q_OBJECT

public:
    explicit EditItemStoreForm(QWidget *parent,
                               mapeditor::controllers::ManageItemStoreController &controller,
                               std::optional<std::reference_wrapper<const std::string>> nameToEdit);
private:
    Ui::editItemStoreFormClass ui;
    mapeditor::controllers::ManageItemStoreController &m_controller;
	bool m_isEditMode;
    const std::string m_itemStoreNameToEdit;
    void connectUIActions();
    bool loadExistingItemStoreToForm();
    void onPushButtonOKClick();
    void onPushButtonOpenFileClick();
};

#endif // EDITITEMSTOREFORM_H
