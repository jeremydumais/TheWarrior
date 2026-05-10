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
m_controller(resourcesPath, textures, m_texturePixmapProvider),
m_timerAnimateNPC(this) {
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

SelectNPCTextureForm::SelectNPCTextureResult SelectNPCTextureForm::getResult() const {
    return m_result;
}

void SelectNPCTextureForm::connectUIActions() {
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &SelectNPCTextureForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &SelectNPCTextureForm::onPushButtonOKClick);
    connect(ui.comboBoxTextureName, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SelectNPCTextureForm::onComboBoxTextureNameChanged);
    connect(ui.listViewNPCAvailable->selectionModel(), &QItemSelectionModel::currentChanged, this, &SelectNPCTextureForm::onListViewNPCAvailableCurrentChanged);
    connect(&m_timerAnimateNPC, &QTimer::timeout, this, &SelectNPCTextureForm::ontimerAnimateNPCTimeout);
}

void SelectNPCTextureForm::onPushButtonCancelClick() {
    reject();
}

void SelectNPCTextureForm::onPushButtonOKClick() {
    const auto qIndex = ui.listViewNPCAvailable->currentIndex();
    if (qIndex.isValid()) {
        m_result.baseTextureIndex = qIndex.data(Qt::UserRole + 1).toInt();
        m_result.textureName = ui.comboBoxTextureName->currentText().toStdString();
        accept();
    }
}

void SelectNPCTextureForm::onComboBoxTextureNameChanged(int index) {
    ui.labelErrorLoading->clear();
    if (index != -1) {
        const auto availableNPC = m_controller.getAvailableNPCs(ui.comboBoxTextureName->currentText().toStdString());
        if (availableNPC.success) {
            size_t indexNPC = 1;
            for (const auto &npcResult : availableNPC.result) {
                auto* item = new QStandardItem(fmt::format("NPC {0}", indexNPC).c_str());
                item->setData(*npcResult.icon.get(), Qt::DecorationRole);
                item->setData(npcResult.baseTextureIndex, Qt::UserRole + 1);
                reinterpret_cast<QStandardItemModel *>(ui.listViewNPCAvailable->model())->appendRow(item);
                indexNPC++;
            }
        } else {
            m_timerAnimateNPC.stop();
            ui.labelErrorLoading->setText(m_controller.getLastError().c_str());
            reinterpret_cast<QStandardItemModel *>(ui.listViewNPCAvailable->model())->clear();
            ui.labelNPCUp->clear();
            ui.labelNPCRight->clear();
            ui.labelNPCLeft->clear();
            ui.labelNPCDown->clear();
        }
    }
}

void SelectNPCTextureForm::onListViewNPCAvailableCurrentChanged(const QModelIndex &current,
                                                                const QModelIndex &) {
    if (current.isValid()) {
        const int baseTextureIndex = current.data(Qt::UserRole + 1).toInt();
        m_animationNPC = m_controller.getNPCAnimationTiles(ui.comboBoxTextureName->currentText().toStdString(),
                                                           baseTextureIndex);
        if (m_animationNPC.success) {
            ui.labelNPCUp->setPixmap(m_animationNPC.walkUp.at(0));
            ui.labelNPCRight->setPixmap(m_animationNPC.walkRight.at(0));
            ui.labelNPCLeft->setPixmap(m_animationNPC.walkLeft.at(0));
            ui.labelNPCDown->setPixmap(m_animationNPC.walkDown.at(0));
            m_animationNPCIndex = 0;
            m_animationDecrease = false;
            m_timerAnimateNPC.start(200);
        } else {
            commoneditor::ui::ErrorMessage::show("Error");
        }
    } else {
        m_timerAnimateNPC.stop();
    }
}

void SelectNPCTextureForm::ontimerAnimateNPCTimeout() {
    if (m_animationNPCIndex >= 2) {
        m_animationDecrease = true;
    }
    if (!m_animationDecrease) {
        m_animationNPCIndex++;
    } else {
        m_animationNPCIndex--;
    }
    ui.labelNPCUp->setPixmap(m_animationNPC.walkUp.at(m_animationNPCIndex));
    ui.labelNPCRight->setPixmap(m_animationNPC.walkRight.at(m_animationNPCIndex));
    ui.labelNPCLeft->setPixmap(m_animationNPC.walkLeft.at(m_animationNPCIndex));
    ui.labelNPCDown->setPixmap(m_animationNPC.walkDown.at(m_animationNPCIndex));
    if (m_animationNPCIndex == 0) {
        m_animationDecrease = false;
    }
}
