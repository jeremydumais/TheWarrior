#include "manageItemStoreForm.hpp"
#include "editItemStoreForm.hpp"
#include "editTextureForm.hpp"

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

void ManageItemStoreForm::onPushButtonAddClick()
{
    //EditItemStoreForm editItemStoreForm(this, m_controller.getResourcesPath(), m_userConfigFolder, m_itemStores);
    //editItemStoreForm.exec();
}

void ManageItemStoreForm::onPushButtonEditClick()
{

}

void ManageItemStoreForm::onPushButtonDeleteClick()
{

}
