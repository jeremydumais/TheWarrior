#include <fmt/core.h>
#include <fmt/format.h>
#include <string>
#include "editNPCForm.hpp"
#include "editNPCFormController.hpp"

EditNPCForm::EditNPCForm(QWidget *parent,
                         const std::string &resourcesPath)
    : QDialog(parent),
    ui(Ui::editNPCFormClass()),
    m_controller(resourcesPath) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    ui.comboBoxDefaultFacing->addItem("Left");
    ui.comboBoxDefaultFacing->addItem("Up");
    ui.comboBoxDefaultFacing->addItem("Right");
    ui.comboBoxDefaultFacing->addItem("Down");
    connectUIActions();
    refreshPositionLabel();
}

bool EditNPCForm::isSpawnPositionPickerModeEnabled() const {
    return m_spawnPositionPickerModeEnabled;
}

void EditNPCForm::restoreFromPicker(const thewarrior::models::Point<> &position) {
    m_spawnPosition = position;
    refreshPositionLabel();
}

void EditNPCForm::connectUIActions() {
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditNPCForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditNPCForm::onPushButtonOKClick);
    connect(ui.pushButtonSpawnPositionPicker, &QPushButton::clicked, this, &EditNPCForm::onPushButtonSpawnPositionPickerClick);
}

void EditNPCForm::refreshPositionLabel() {
    if (m_spawnPosition.x() < 0 || m_spawnPosition.y() < 0) {
        ui.labelSpawnPositionValue->setText("<no value selected>");
    } else {
        ui.labelSpawnPositionValue->setText(fmt::format("({0}, {1})",
                                                        m_spawnPosition.x(),
                                                        m_spawnPosition.y()).c_str());
    }
}

void EditNPCForm::onPushButtonCancelClick() {
    reject();
}

void EditNPCForm::onPushButtonOKClick() {
    accept();
}

void EditNPCForm::onPushButtonSpawnPositionPickerClick() {
    m_spawnPositionPickerModeEnabled = true;
    accept();
}
