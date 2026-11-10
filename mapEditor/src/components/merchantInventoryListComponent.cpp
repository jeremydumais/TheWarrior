#include "merchantInventoryListComponent.hpp"
#include <QStyle>
#include <qwidget.h>
#include <qcheckbox.h>
#include <qnamespace.h>
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "editMerchantInventoryForm.hpp"
#include "errorMessage.hpp"
#include "merchantInventory.hpp"
#include "uiUtils.hpp"
#include "types.hpp"
#include "warningMessage.hpp"

using commoneditor::ui::ErrorMessage;
using commoneditor::ui::WarningMessage;
using commoneditor::ui::UIUtils;
using mapeditor::controllers::GLComponentController;
using mapeditor::controllers::ContainerOfItemStore;
using thewarrior::models::MerchantInventoryType;

MerchantInventoryListComponent::MerchantInventoryListComponent(QWidget *parent,
        MainForm_GLComponent *glComponent,
        GLComponentController *glComponentController)
    : QWidget(parent),
      ui(Ui::MerchantInventoryListComponent()),
      m_controller(glComponentController),
      m_glComponent(glComponent) {
      ui.setupUi(this);
      initializeUIObjects();
      connectUIActions();
}

void MerchantInventoryListComponent::initializeUIObjects() {
    ui.tableWidgetMerchantInventory->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    ui.tableWidgetMerchantInventory->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));
    ui.tableWidgetMerchantInventory->setColumnWidth(0, 160);
    auto *addMenu = new QMenu(ui.pushButtonAddMerchantInventory);
    addMenu->addAction("Weapons and Armors", this, [this]() {
        onPushButtonAddMerchantInventory(thewarrior::models::MerchantInventoryType::WeaponsAndArmors);
    });
    addMenu->addAction("Stats Items", this, [this]() {
        onPushButtonAddMerchantInventory(thewarrior::models::MerchantInventoryType::StatsItems);
    });
    ui.pushButtonAddMerchantInventory->setMenu(addMenu);
}

void MerchantInventoryListComponent::connectUIActions() {
    connect(ui.pushButtonEditMerchantInventory, &QPushButton::clicked, this, &MerchantInventoryListComponent::onPushButtonEditMerchantInventoryClick);
    connect(ui.pushButtonDeleteMerchantInventory, &QPushButton::clicked, this, &MerchantInventoryListComponent::onPushButtonDeleteMerchantInventoryClick);
    connect(ui.tableWidgetMerchantInventory, &QTableWidget::itemDoubleClicked, this, &MerchantInventoryListComponent::onPushButtonEditMerchantInventoryClick);
    tableWidgetMerchantInventoryKeyWatcher.installOn(ui.tableWidgetMerchantInventory);
    connect(&tableWidgetMerchantInventoryKeyWatcher, &QTableWidgetKeyPressWatcher::keyPressed, this, &MerchantInventoryListComponent::onTableWidgetMerchantInventoryKeyPressEvent);
}

void MerchantInventoryListComponent::refreshMerchantInventories() {
    auto getInventoryTypeStr = [](MerchantInventoryType type) {
        if (type == MerchantInventoryType::WeaponsAndArmors) {
            return "Weapons and Armors";
        }
        return "Stats Items";
    };
    ui.tableWidgetMerchantInventory->model()->removeRows(0, ui.tableWidgetMerchantInventory->rowCount());
    int index {0};
    for (const auto &merchantInventory : m_controller.getMerchantInventories()) {
        ui.tableWidgetMerchantInventory->insertRow(index);
        ui.tableWidgetMerchantInventory->setItem(index, 0, new QTableWidgetItem(merchantInventory.name.c_str()));
        ui.tableWidgetMerchantInventory->setItem(index, 1, new QTableWidgetItem(getInventoryTypeStr(merchantInventory.inventoryType)));
        index++;
    }
}

void MerchantInventoryListComponent::disableFieldsChangeEvent() {
    m_disableFieldsChangedEvent = true;
}

void MerchantInventoryListComponent::enableFieldsChangeEvent() {
    m_disableFieldsChangedEvent = false;
}

/*
std::vector<MonsterZoneDTO> MonsterZoneListComponent::getMonsterZones() const {
    return m_controller.getMonsterZones();
}

std::optional<const MonsterZoneDTO> MonsterZoneListComponent::getSelectedMonsterZoneInMonsterZoneList() const {
    if (ui.tableWidgetMonsterZone->selectionModel()->hasSelection()) {
        // Find the selected monster zone
        const auto selectedRow = ui.tableWidgetMonsterZone->selectionModel()->selectedRows()[0];
        auto selectedItemName { selectedRow.sibling(selectedRow.row(), 1).data().toString().toStdString() };
        return m_controller.getMonsterZoneByName(selectedItemName);
    } else {
        return std::nullopt;
    }
}*/

void MerchantInventoryListComponent::setItemStores(const std::shared_ptr<ContainerOfItemStore> &itemStores) {
    m_controller.setItemStores(itemStores);
}

void MerchantInventoryListComponent::setResourcesPath(const std::string &resourcesPath) {
    m_resourcesPath = resourcesPath;
}
/*
void MonsterZoneListComponent::confirmValidityOfOneMonsterZoneCheckBox() {
    const auto &zones = m_controller.getMonsterZones();
    if (zones.size() != 1 &&
        ui.checkBoxOneMonsterZoneForAllTheMap->checkState() == Qt::CheckState::Checked) {
        ui.checkBoxOneMonsterZoneForAllTheMap->setCheckState(Qt::CheckState::Unchecked);
    }
}

std::string MonsterZoneListComponent::getMonsterZoneColor(const std::string &zoneName) const {
    const auto &zone = m_controller.getMonsterZoneByName(zoneName);
    if (zone.has_value()) {
        return zone->m_colorValue;
    }
    return "";
}

bool MonsterZoneListComponent::isMonsterZonesEmpty() const {
    return m_controller.isMonsterZonesEmpty();
}

bool MonsterZoneListComponent::isOnlyOneMonsterZoneChecked() const {
    return ui.checkBoxOneMonsterZoneForAllTheMap->checkState() == Qt::CheckState::Checked;
}*/

void MerchantInventoryListComponent::onPushButtonAddMerchantInventory(MerchantInventoryType inventoryType) {
    m_glComponent->stopAutoUpdate();
    const std::vector<std::string> alreadyUsedMerchantInventoryNames = m_controller.getAlreadyUsedMerchantInventoryNames();
    EditMerchantInventoryForm formEditMerchantInventory(this,
            m_controller.getItemStores(),
            m_resourcesPath,
            inventoryType,
            std::nullopt,
            alreadyUsedMerchantInventoryNames);
    UIUtils::centerToScreen(&formEditMerchantInventory);
    if (formEditMerchantInventory.exec() == QDialog::Accepted) {
        emit merchantInventoryAdded(formEditMerchantInventory.getResult());
    }
    m_glComponent->startAutoUpdate();
}

void MerchantInventoryListComponent::onPushButtonEditMerchantInventoryClick() {
    m_glComponent->stopAutoUpdate();
    if (auto merchantInvName = getSelectedMerchantInventoryName(); merchantInvName.has_value()) {
        auto merchantInventory = m_controller.getMerchantInventoryByName(merchantInvName.value());
        if (merchantInventory.has_value()) {
            const std::vector<std::string> alreadyUsedMerchantInventoryNames = m_controller.getAlreadyUsedMerchantInventoryNames();
            EditMerchantInventoryForm formEditMerchantInventory(this,
                    m_controller.getItemStores(),
                    m_resourcesPath,
                    merchantInventory->get().inventoryType,
                    merchantInventory.value(),
                    alreadyUsedMerchantInventoryNames);
            UIUtils::centerToScreen(&formEditMerchantInventory);
            if (formEditMerchantInventory.exec() == QDialog::Accepted) {
                // Ensure you cannot rename a MerchantInventory that is used in a ConversationScenario
                auto result = formEditMerchantInventory.getResult();
                if (m_controller.isMerchantInventoryUsed(merchantInvName.value()) && merchantInvName.value() != result.name) {
                    WarningMessage::show(fmt::format("The merchant inventory {} cannnot be renamed to {} because it is currently used by a conversation action. The existing name will be kept.",
                                                     merchantInvName.value(), result.name));
                    result.name = merchantInvName.value();
                }
                emit merchantInventoryUpdated(merchantInvName.value(), result);
            }
        }
    }
    m_glComponent->startAutoUpdate();
}

void MerchantInventoryListComponent::onPushButtonDeleteMerchantInventoryClick() {
    if (auto merchantInvName = getSelectedMerchantInventoryName(); merchantInvName.has_value()) {
        auto merchantInventory = m_controller.getMerchantInventoryByName(merchantInvName.value());
        if (merchantInventory.has_value()) {
            QMessageBox msgBox;
            msgBox.setText(fmt::format("Are you sure you want to delete the merchant inventory {0}?", merchantInventory->get().name).c_str());
            msgBox.setWindowTitle("Confirmation");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            if (msgBox.exec() == QMessageBox::Yes) {
                if (m_controller.isMerchantInventoryUsed(merchantInventory->get().name)) {
                    ErrorMessage::show(fmt::format("The merchant inventory {} is used by a conversation action and cannot be deleted.",
                                                   merchantInventory->get().name));
                    return;
                }
                emit merchantInventoryDeleted(merchantInventory->get().name);
            }
        }
    }
}

void MerchantInventoryListComponent::onTableWidgetMerchantInventoryKeyPressEvent(int key, int /*row*/, int /*column*/) {
    if (key == Qt::Key_Delete) {
        onPushButtonDeleteMerchantInventoryClick();
    }
}

std::optional<std::string> MerchantInventoryListComponent::getSelectedMerchantInventoryName() const {
    auto selectedRows = ui.tableWidgetMerchantInventory->selectionModel()->selectedRows();
    if (selectedRows.count() == 1) {
        return selectedRows[0].data().toString().toStdString();
    }
    return std::nullopt;
}
