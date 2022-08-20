#include "editItemStoreForm.hpp"
#include "configurationManager.hpp"
#include "errorMessage.hpp"
#include <qfiledialog.h>
#include <qfileinfo.h>

using namespace commoneditor::ui;
using namespace thewarrior::storage;

EditItemStoreForm::EditItemStoreForm(QWidget *parent,
                                     const std::string &resourcesPath,
                                     const std::string &userConfigFolder,
                                     const std::vector<ItemStoreInfo> &itemStores)
    : QDialog(parent),
    ui(Ui::editItemStoreFormClass()),
    m_resourcesPath(resourcesPath),
    m_userConfigFolder(userConfigFolder),
    m_itemStores(itemStores)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    connectUIActions();
}

void EditItemStoreForm::connectUIActions()
{
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditItemStoreForm::onPushButtonOKClick);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditItemStoreForm::reject);
    connect(ui.pushButtonOpenFile, &QPushButton::clicked, this, &EditItemStoreForm::onPushButtonOpenFileClick);
}

void EditItemStoreForm::onPushButtonOKClick()
{
    //TODO For edition, receive the current item store name to be edited
    //
    //TODO Check the the item store name is not already used
    //TODO Load the configuration file
    //TODO Save the itemstore
    ConfigurationManager configManager(m_userConfigFolder + "config.json");
    if (configManager.load())
    {
        //configManager.setStringValue(MainForm::THEME_PATH, "Dark");
        //setAppStylesheet(configManager.getStringValue(MainForm::THEME_PATH));
        //if (!configManager.save())
        //{
            //ErrorMessage::show("An error occurred while saving the configuration file.",
                    //configManager.getLastError());
        //}
    }
    else
    {
        ErrorMessage::show("An error occurred while loading the configuration file.",
                configManager.getLastError());
    }
    accept();
}

void EditItemStoreForm::onPushButtonOpenFileClick()
{
    QString fullFilePath { QFileDialog::getOpenFileName(this,
            tr("Open Item Store"),
            m_resourcesPath.c_str(),
            tr("Item Store (*.itm)")) };
    QFileInfo fileInfo(fullFilePath);
    std::string filename { fileInfo.fileName().toStdString() };
    ui.lineEditFilename->setText(filename.c_str());
}
