#include "manageItemStoreForm.hpp"
#include "editItemStoreForm.hpp"
#include "editTextureForm.hpp"
#include "errorMessage.hpp"
#include <fmt/format.h>
#include <optional>
#include <qdialog.h>
#include <qmessagebox.h>

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
    connect(ui.pushButtonClose, &QPushButton::clicked, this, &ManageItemStoreForm::close);
    connect(ui.pushButtonAdd, &QPushButton::clicked, this, &ManageItemStoreForm::onPushButtonAddClick);
    connect(ui.pushButtonEdit, &QPushButton::clicked, this, &ManageItemStoreForm::onPushButtonEditClick);
    connect(ui.pushButtonDelete, &QPushButton::clicked, this, &ManageItemStoreForm::onPushButtonDeleteClick);
    connect(ui.tableWidgetItem, &QTableWidget::itemDoubleClicked, this, &ManageItemStoreForm::onPushButtonEditClick);
    tableWidgetItemKeyWatcher.installOn(ui.tableWidgetItem);
    connect(&tableWidgetItemKeyWatcher, &QTableWidgetKeyPressWatcher::keyPressed, this, &ManageItemStoreForm::onTableWidgetItemKeyPressEvent);
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
if (const auto itemStoreName = getSelectedItemStoreName(); itemStoreName.has_value()) {
        EditItemStoreForm editItemStoreForm(this, m_controller, itemStoreName.value());
        if (editItemStoreForm.exec() == QDialog::Accepted) {
            refreshItemStoreList();
        }
    }
}

void ManageItemStoreForm::onPushButtonDeleteClick()
{
    if (const auto itemStoreName = getSelectedItemStoreName(); itemStoreName.has_value()) {
        QMessageBox msgBox;
        msgBox.setText(fmt::format("Are you sure you want to delete the item store {0}?", itemStoreName.value()).c_str());
        msgBox.setWindowTitle("Confirmation");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Yes) {
            if (m_controller.deleteItemStore(itemStoreName.value())) {
                if (!m_controller.saveItemStore()) {
                    ErrorMessage::show(m_controller.getLastError());
                }
                refreshItemStoreList();
            }
            else {
                ErrorMessage::show(m_controller.getLastError());
            }
        }
    }
}

void ManageItemStoreForm::onTableWidgetItemKeyPressEvent(int key, int, int)
{
    if (key == Qt::Key_Delete) {
        onPushButtonDeleteClick();
    }
}

std::optional<std::string> ManageItemStoreForm::getSelectedItemStoreName() const
{
    auto selectedRows = ui.tableWidgetItem->selectionModel()->selectedRows();
    if (selectedRows.count() == 1) {
        return selectedRows[0].data().toString().toStdString();
    }
    return std::nullopt;
}
