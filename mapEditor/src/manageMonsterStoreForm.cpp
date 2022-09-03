#include "manageMonsterStoreForm.hpp"
#include "editMonsterStoreForm.hpp"
#include "editTextureForm.hpp"
#include "errorMessage.hpp"
#include <exception>
#include <fmt/format.h>
#include <optional>
#include <qdialog.h>
#include <qmessagebox.h>

using namespace commoneditor::ui;

ManageMonsterStoreForm::ManageMonsterStoreForm(QWidget *parent,
                                         const std::string &resourcesPath,
                                         const std::string &userConfigFolder)
    : QDialog(parent),
    ui(Ui::manageMonsterStoreFormClass()),
    m_controller(resourcesPath, userConfigFolder)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    initializeMonsterTable();
    connectUIActions();
    if (!m_controller.loadMonsterStore()) {
        ErrorMessage::show(m_controller.getLastError());
    }
    refreshMonsterStoreList();
}

void ManageMonsterStoreForm::connectUIActions()
{
    connect(ui.pushButtonClose, &QPushButton::clicked, this, &ManageMonsterStoreForm::close);
    connect(ui.pushButtonAdd, &QPushButton::clicked, this, &ManageMonsterStoreForm::onPushButtonAddClick);
    connect(ui.pushButtonEdit, &QPushButton::clicked, this, &ManageMonsterStoreForm::onPushButtonEditClick);
    connect(ui.pushButtonDelete, &QPushButton::clicked, this, &ManageMonsterStoreForm::onPushButtonDeleteClick);
    connect(ui.tableWidgetMonsterStore, &QTableWidget::itemDoubleClicked, this, &ManageMonsterStoreForm::onPushButtonEditClick);
    tableWidgetMonsterKeyWatcher.installOn(ui.tableWidgetMonsterStore);
    connect(&tableWidgetMonsterKeyWatcher, &QTableWidgetKeyPressWatcher::keyPressed, this, &ManageMonsterStoreForm::onTableWidgetMonsterKeyPressEvent);
}

void ManageMonsterStoreForm::initializeMonsterTable()
{
    ui.tableWidgetMonsterStore->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    ui.tableWidgetMonsterStore->setHorizontalHeaderItem(1, new QTableWidgetItem("Filename"));
    ui.tableWidgetMonsterStore->setColumnWidth(0, 150);
    ui.tableWidgetMonsterStore->setColumnWidth(1, 250);
}

void ManageMonsterStoreForm::refreshMonsterStoreList()
{
    ui.tableWidgetMonsterStore->model()->removeRows(0, ui.tableWidgetMonsterStore->rowCount());
    int index = 0;
    for(const auto &item : m_controller.getMonsterStores()) {
        ui.tableWidgetMonsterStore->insertRow(index);
        ui.tableWidgetMonsterStore->setItem(index, 0, new QTableWidgetItem(item.name.c_str()));
        ui.tableWidgetMonsterStore->setItem(index, 1, new QTableWidgetItem(item.filename.c_str()));
    }
}

void ManageMonsterStoreForm::onPushButtonAddClick()
{
    EditMonsterStoreForm editMonsterStoreForm(this, m_controller, std::nullopt);
    if (editMonsterStoreForm.exec() == QDialog::Accepted) {
        refreshMonsterStoreList();
    }
}

void ManageMonsterStoreForm::onPushButtonEditClick()
{
    if (const auto monsterStoreName = getSelectedMonsterStoreName(); monsterStoreName.has_value()) {
        EditMonsterStoreForm editMonsterStoreForm(this, m_controller, monsterStoreName.value());
        if (editMonsterStoreForm.exec() == QDialog::Accepted) {
            refreshMonsterStoreList();
        }
    }
}

void ManageMonsterStoreForm::onPushButtonDeleteClick()
{
    if (const auto itemStoreName = getSelectedMonsterStoreName(); itemStoreName.has_value()) {
        QMessageBox msgBox;
        msgBox.setText(fmt::format("Are you sure you want to delete the item store {0}?", itemStoreName.value()).c_str());
        msgBox.setWindowTitle("Confirmation");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Yes) {
            if (m_controller.deleteMonsterStore(itemStoreName.value())) {
                if (!m_controller.saveMonsterStore()) {
                    ErrorMessage::show(m_controller.getLastError());
                }
                refreshMonsterStoreList();
            }
            else {
                ErrorMessage::show(m_controller.getLastError());
            }
        }
    }
}

void ManageMonsterStoreForm::onTableWidgetMonsterKeyPressEvent(int key, int, int)
{
    if (key == Qt::Key_Delete) {
        onPushButtonDeleteClick();
    }
}

std::optional<std::string> ManageMonsterStoreForm::getSelectedMonsterStoreName() const
{
    auto selectedRows = ui.tableWidgetMonsterStore->selectionModel()->selectedRows();
    if (selectedRows.count() == 1) {
        return selectedRows[0].data().toString().toStdString();
    }
    return std::nullopt;
}
