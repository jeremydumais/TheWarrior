#include <fmt/core.h>
#include <fmt/format.h>
#include <qstandarditemmodel.h>
#include <cstddef>
#include <string>
#include <vector>
#include "selectNPCTextureForm.hpp"
#include "errorMessage.hpp"
#include "selectNPCTextureFormController.hpp"

SelectNPCTextureForm::SelectNPCTextureForm(QWidget *parent,
                                           const std::string &resourcesPath,
                                           const std::vector<thewarrior::models::Texture> &textures)
    : QDialog(parent),
    ui(Ui::selectNPCTextureFormClass()),
    m_controller(resourcesPath, textures, m_texturePixmapProvider) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    for (const auto &textureName : m_controller.getTextureNames()) {
        ui.comboBoxTextureName->addItem(textureName.c_str());
    }
    auto* model = new QStandardItemModel(ui.listViewNPCAvailable);
    ui.listViewNPCAvailable->setModel(model);
    connectUIActions();
    onComboBoxTextureNameChanged(ui.comboBoxTextureName->currentIndex());
}

void SelectNPCTextureForm::connectUIActions() {
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &SelectNPCTextureForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &SelectNPCTextureForm::onPushButtonOKClick);
    connect(ui.comboBoxTextureName, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SelectNPCTextureForm::onComboBoxTextureNameChanged);
    connect(ui.listViewNPCAvailable->selectionModel(), &QItemSelectionModel::currentChanged, this, &SelectNPCTextureForm::onListViewNPCAvailableCurrentChanged);
}

void SelectNPCTextureForm::onPushButtonCancelClick() {
    reject();
}

void SelectNPCTextureForm::onPushButtonOKClick() {
    accept();
}

void SelectNPCTextureForm::onComboBoxTextureNameChanged(int index) {
    ui.labelErrorLoading->clear();
    if (index != -1) {
        const auto availableNPC = m_controller.getAvailableNPCs(ui.comboBoxTextureName->currentText().toStdString());
        if (availableNPC.success) {
            size_t indexNPC = 1;
            for (const auto &npcResult : availableNPC.result) {
                auto* item = new QStandardItem(fmt::format("NPC {0}", indexNPC).c_str());
                item->setData(npcResult.icon, Qt::DecorationRole);
                item->setData(npcResult.baseTextureIndex, Qt::UserRole + 1);
                reinterpret_cast<QStandardItemModel *>(ui.listViewNPCAvailable->model())->appendRow(item);
                indexNPC++;
            }
        } else {
            ui.labelErrorLoading->setText(m_controller.getLastError().c_str());
        }
    }
}

void SelectNPCTextureForm::onListViewNPCAvailableCurrentChanged(const QModelIndex &current,
                                                                const QModelIndex &) {
    const int baseTextureIndex = current.data(Qt::UserRole + 1).toInt();
    const auto animationResult = m_controller.getNPCAnimationTiles(ui.comboBoxTextureName->currentText().toStdString(),
                                                                   baseTextureIndex);
    if (animationResult.success) {
        ui.labelNPCUp->setPixmap(animationResult.walkUp.at(0));
        ui.labelNPCRight->setPixmap(animationResult.walkRight.at(0));
        ui.labelNPCLeft->setPixmap(animationResult.walkLeft.at(0));
        ui.labelNPCDown->setPixmap(animationResult.walkDown.at(0));
    } else {
        commoneditor::ui::ErrorMessage::show("Error");
    }
}
