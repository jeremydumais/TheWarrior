#include "manageItemStoreForm.hpp"
#include "editItemStoreForm.hpp"
#include "editTextureForm.hpp"
#include "errorMessage.hpp"
#include <optional>
#include <qdialog.h>

using namespace commoneditor::ui;

ManageItemStoreForm::ManageItemStoreForm(QWidget *parent,
                                         const std::string &resourcesPath,
                                         const std::string &userConfigFolder)
    : QDialog(parent),
    ui(Ui::manageItemStoreFormClass()),
    m_controller(resourcesPath, userConfigFolder)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    initializeItemTable();
    connectUIActions();
    if (!m_controller.loadItemStore()) {
        ErrorMessage::show(m_controller.getLastError());
    }
    refreshItemStoreList();
}

void ManageItemStoreForm::connectUIActions()
{
    connect(ui.pushButtonClose, &QPushButton::clicked, this, std::terminate);
    connect(ui.pushButtonAdd, &QPushButton::clicked, this, &ManageItemStoreForm::onPushButtonAddClick);
    connect(ui.pushButtonEdit, &QPushButton::clicked, this, &ManageItemStoreForm::onPushButtonEditClick);
    connect(ui.pushButtonDelete, &QPushButton::clicked, this, &ManageItemStoreForm::onPushButtonDeleteClick);
}

void ManageItemStoreForm::initializeItemTable()
{
    ui.tableWidgetItem->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    ui.tableWidgetItem->setHorizontalHeaderItem(1, new QTableWidgetItem("Filename"));
    ui.tableWidgetItem->setColumnWidth(0, 150);
    ui.tableWidgetItem->setColumnWidth(1, 250);
}

void ManageItemStoreForm::refreshItemStoreList()
{
    ui.tableWidgetItem->model()->removeRows(0, ui.tableWidgetItem->rowCount());
    int index = 0;
    for(const auto &item : m_controller.getItemStores()) {
        ui.tableWidgetItem->insertRow(index);
        ui.tableWidgetItem->setItem(index, 0, new QTableWidgetItem(item.name.c_str()));
        ui.tableWidgetItem->setItem(index, 1, new QTableWidgetItem(item.filename.c_str()));
    }
}

void ManageItemStoreForm::onPushButtonAddClick()
{
    EditItemStoreForm editItemStoreForm(this, m_controller, std::nullopt);
    if (editItemStoreForm.exec() == QDialog::Accepted) {
        refreshItemStoreList();
    }
}

void ManageItemStoreForm::onPushButtonEditClick()
{

}

void ManageItemStoreForm::onPushButtonDeleteClick()
{

}
