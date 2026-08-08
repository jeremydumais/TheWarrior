#pragma once

#include <QtWidgets/QTableWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <qcheckbox.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "glComponentController.hpp"
#include "mainForm_GLComponent.hpp"
#include "merchantInventory.hpp"
#include "merchantInventoryDTO.hpp"
#include "merchantInventoryListComponentController.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include "ui_merchantInventoryListComponent.h"
#include "types.hpp"

class MerchantInventoryListComponent : public QWidget {
    Q_OBJECT

 public:
    MerchantInventoryListComponent(QWidget *parent,
            MainForm_GLComponent *glComponent,
            mapeditor::controllers::GLComponentController *glComponentController);
    void initializeUIObjects();
    void connectUIActions();
    void refreshMerchantInventories();
    void disableFieldsChangeEvent();
    void enableFieldsChangeEvent();
    /*std::vector<mapeditor::controllers::MonsterZoneDTO> getMonsterZones() const;
    std::optional<const mapeditor::controllers::MonsterZoneDTO> getSelectedMonsterZoneInMonsterZoneList() const;*/
    void setItemStores(const std::shared_ptr<mapeditor::controllers::ContainerOfItemStore> &itemStores);
    void setResourcesPath(const std::string &resourcesPath);
 signals:
    void merchantInventoryAdded(mapeditor::controllers::MerchantInventoryDTO merchantInventoryDTO);
    void merchantInventoryUpdated(const std::string &name, mapeditor::controllers::MerchantInventoryDTO merchantInventoryDTO);
    void merchantInventoryDeleted(const std::string &name);

 private:
    Ui::MerchantInventoryListComponent ui;
    mapeditor::controllers::MerchantInventoryListComponentController m_controller;
    std::string m_resourcesPath;
    QTableWidgetKeyPressWatcher tableWidgetMerchantInventoryKeyWatcher;
    MainForm_GLComponent *m_glComponent;
    bool m_disableFieldsChangedEvent = false;
    void onPushButtonAddMerchantInventory(thewarrior::models::MerchantInventoryType inventoryType);
    void onPushButtonEditMerchantInventoryClick();
    void onPushButtonDeleteMerchantInventoryClick();
    void onTableWidgetMerchantInventoryKeyPressEvent(int key, int row, int column);
    std::optional<std::string> getSelectedMerchantInventoryName() const;
};
