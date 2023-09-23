#include "editItemStoreForm.hpp"
#include "errorMessage.hpp"
#include "itemStoreInfo.hpp"
#include <qfiledialog.h>
#include <qfileinfo.h>

using namespace commoneditor::ui;
using namespace mapeditor::controllers;

EditItemStoreForm::EditItemStoreForm(QWidget *parent,
                                     ManageItemStoreController &controller,
                                     std::optional<std::reference_wrapper<const std::string>> nameToEdit)
    : QDialog(parent),
    ui(Ui::editItemStoreFormClass()),
    m_controller(controller),
    m_isEditMode(nameToEdit.has_value()),
    m_itemStoreNameToEdit(nameToEdit.has_value() ? nameToEdit.value().get() : "")
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    connectUIActions();
    if (m_isEditMode) {
        this->setWindowTitle("Edit item store");
        if (!loadExistingItemStoreToForm()) {
            ErrorMessage::show("Unable to locate the item store to load.");
        }
    }
    else {
        this->setWindowTitle("Add item store");
    }
}

void EditItemStoreForm::connectUIActions()
{
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditItemStoreForm::onPushButtonOKClick);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditItemStoreForm::reject);
    connect(ui.pushButtonOpenFile, &QPushButton::clicked, this, &EditItemStoreForm::onPushButtonOpenFileClick);
}

bool EditItemStoreForm::loadExistingItemStoreToForm()
{
    //Locate the item store from it's name
    auto itemStore = m_controller.findItemStore(m_itemStoreNameToEdit);
    if (!itemStore.has_value()) {
        return false;
    }
    //Fill the form fields
    ui.lineEditName->setText(itemStore->name.c_str());
    ui.lineEditFilename->setText(itemStore->filename.c_str());
    return true;
}

void EditItemStoreForm::onPushButtonOKClick()
{
    ItemStoreInfo item = { .name = ui.lineEditName->text().toStdString(),
                           .filename = ui.lineEditFilename->text().toStdString() };
    if (!m_isEditMode && !m_controller.addItemStore(item)) {
        ErrorMessage::show(m_controller.getLastError());
        return;
    }
    if (m_isEditMode && !m_controller.updateItemStore(m_itemStoreNameToEdit, item)) {
        ErrorMessage::show(m_controller.getLastError());
        return;
    }
    if (m_controller.saveItemStore()) {
        accept();
    }
    else {
        ErrorMessage::show(m_controller.getLastError());
    }
}

void EditItemStoreForm::onPushButtonOpenFileClick()
{
    QString fullFilePath { QFileDialog::getOpenFileName(this,
            tr("Open Item Store"),
            m_controller.getResourcesPath().c_str(),
            tr("Item Store (*.itm)")) };
    QFileInfo fileInfo(fullFilePath);
    std::string filename { fileInfo.fileName().toStdString() };
    ui.lineEditFilename->setText(filename.c_str());
}
