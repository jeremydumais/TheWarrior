#include "editMonsterStoreForm.hpp"
#include "errorMessage.hpp"
#include "monsterStoreInfo.hpp"
#include <qfiledialog.h>
#include <qfileinfo.h>

using namespace commoneditor::ui;
using namespace mapeditor::controllers;

EditMonsterStoreForm::EditMonsterStoreForm(QWidget *parent,
                                     ManageMonsterStoreController &controller,
                                     std::optional<std::reference_wrapper<const std::string>> nameToEdit)
    : QDialog(parent),
    ui(Ui::editMonsterStoreFormClass()),
    m_controller(controller),
    m_isEditMode(nameToEdit.has_value()),
    m_monsterStoreNameToEdit(nameToEdit.has_value() ? nameToEdit.value().get() : "")
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    connectUIActions();
    if (m_isEditMode) {
        this->setWindowTitle("Edit monster store");
        if (!loadExistingMonsterStoreToForm()) {
            ErrorMessage::show("Unable to locate the monster store to load.");
        }
    }
    else {
        this->setWindowTitle("Add monster store");
    }
}

void EditMonsterStoreForm::connectUIActions()
{
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditMonsterStoreForm::onPushButtonOKClick);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditMonsterStoreForm::reject);
    connect(ui.pushButtonOpenFile, &QPushButton::clicked, this, &EditMonsterStoreForm::onPushButtonOpenFileClick);
}

bool EditMonsterStoreForm::loadExistingMonsterStoreToForm()
{
    //Locate the monster store from it's name
    auto monsterStore = m_controller.findMonsterStore(m_monsterStoreNameToEdit);
    if (!monsterStore.has_value()) {
        return false;
    }
    //Fill the form fields
    ui.lineEditName->setText(monsterStore->name.c_str());
    ui.lineEditFilename->setText(monsterStore->filename.c_str());
    return true;
}

void EditMonsterStoreForm::onPushButtonOKClick()
{
    MonsterStoreInfo monster = { .name = ui.lineEditName->text().toStdString(),
                           .filename = ui.lineEditFilename->text().toStdString() };
    if (!m_isEditMode && !m_controller.addMonsterStore(monster)) {
        ErrorMessage::show(m_controller.getLastError());
        return;
    }
    if (m_isEditMode && !m_controller.updateMonsterStore(m_monsterStoreNameToEdit, monster)) {
        ErrorMessage::show(m_controller.getLastError());
        return;
    }
    if (m_controller.saveMonsterStore()) {
        accept();
    }
    else {
        ErrorMessage::show(m_controller.getLastError());
    }
}

void EditMonsterStoreForm::onPushButtonOpenFileClick()
{
    QString fullFilePath { QFileDialog::getOpenFileName(this,
            tr("Open Monster Store"),
            m_controller.getResourcesPath().c_str(),
            tr("Monster Store (*.mon)")) };
    QFileInfo fileInfo(fullFilePath);
    std::string filename { fileInfo.fileName().toStdString() };
    ui.lineEditFilename->setText(filename.c_str());
}
