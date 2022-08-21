#include "editItemStoreForm.hpp"
#include "errorMessage.hpp"
#include <qfiledialog.h>
#include <qfileinfo.h>

using namespace commoneditor::ui;

EditItemStoreForm::EditItemStoreForm(QWidget *parent,
                                     const std::string &resourcesPath,
                                     const std::string &userConfigFolder,
                                     const std::vector<mapeditor::controllers::ItemStoreInfo> &itemStores)
    : QDialog(parent),
    ui(Ui::editItemStoreFormClass()),
    m_controller(resourcesPath, userConfigFolder, itemStores)
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
