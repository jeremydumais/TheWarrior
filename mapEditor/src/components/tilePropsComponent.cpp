#include <fmt/format.h>
#include <qtablewidget.h>
#include <algorithm>
#include <string>
#include <vector>
#include "glComponentController.hpp"
#include "mapTile.hpp"
#include "mapTileDTO.hpp"
#include "mapTileTriggerDTO.hpp"
#include "tilePropsComponent.hpp"
#include "editMapTileTriggerForm.hpp"
#include "errorMessage.hpp"
#include "mapTileTriggerEventConverter.hpp"
#include "uiUtils.hpp"

using commoneditor::ui::ErrorMessage;
using commoneditor::ui::UIUtils;
using mapeditor::controllers::GLComponentController;
using mapeditor::controllers::MapTileDTO;
using mapeditor::controllers::MapTileTriggerDTO;
using thewarrior::models::MapTile;
using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerEventConverter;
using thewarrior::models::Point;

void setEnabledWidgetsInLayout(QLayout *layout, bool enabled);
bool isChildWidgetOfAnyLayout(QLayout *layout, QWidget *widget);

TilePropsComponent::TilePropsComponent(QWidget *parent,
        MainForm_GLComponent *glComponent,
        GLComponentController *glComponentController)
    : QWidget(parent),
      ui(Ui::TilePropsComponent()),
      m_controller(glComponentController),
      m_glComponent(glComponent),
      m_disableFieldsChangedEvent(false) {
      ui.setupUi(this);
      connectUIActions();
      onTileUnselected();
}

void TilePropsComponent::connectUIActions() {
    connect(m_glComponent,
            &MainForm_GLComponent::tileSelected,
            this,
            &TilePropsComponent::onTileSelected);
    connect(m_glComponent,
            &MainForm_GLComponent::tileUnselected,
            this,
            &TilePropsComponent::onTileUnselected);
    connect(ui.lineEditTexName,
            &QLineEdit::textChanged,
            this,
            &TilePropsComponent::onLineEditTexNameTextChanged);
    connect(ui.spinBoxTexIndex,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this,
            &TilePropsComponent::onSpinBoxTexIndexValueChanged);
    connect(ui.lineEditObjTexName,
            &QLineEdit::textChanged,
            this,
            &TilePropsComponent::onLineEditObjTexNameTextChanged);
    connect(ui.spinBoxObjTexIndex,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this,
            &TilePropsComponent::onSpinBoxObjTexIndexValueChanged);
    connect(ui.checkBoxTileCanSteppedOn,
            &QCheckBox::stateChanged,
            this,
            &TilePropsComponent::onCheckBoxTileCanSteppedOnChanged);
    connect(ui.checkBoxObjectAbovePlayer,
            &QCheckBox::stateChanged,
            this,
            &TilePropsComponent::onCheckBoxObjectAbovePlayerChanged);
    connect(ui.checkBoxIsWallToClimb,
            &QCheckBox::stateChanged,
            this,
            &TilePropsComponent::onCheckBoxIsWallToClimbChanged);
    connect(ui.pushButtonAddTileEvent,
            &QPushButton::clicked,
            this,
            &TilePropsComponent::onPushButtonAddTileEventClick);
    connect(ui.pushButtonEditTileEvent,
            &QPushButton::clicked,
            this,
            &TilePropsComponent::onPushButtonEditTileEventClick);
    connect(ui.pushButtonDeleteTileEvent,
            &QPushButton::clicked,
            this,
            &TilePropsComponent::onPushButtonDeleteTileEventClick);
    connect(ui.tableWidgetMapTileTriggers,
            &QTableWidget::itemDoubleClicked,
            this,
            &TilePropsComponent::onPushButtonEditTileEventClick);
    tableWidgetMapTileTriggersKeyWatcher.installOn(ui.tableWidgetMapTileTriggers);
    connect(&tableWidgetMapTileTriggersKeyWatcher,
            &QTableWidgetKeyPressWatcher::keyPressed,
            this,
            &TilePropsComponent::onTableWidgetMapTileTriggersKeyPressEvent);
}

void TilePropsComponent::reset() {
    ui.labelTileCoordXY->clear();
    ui.lineEditTexName->clear();
    ui.spinBoxTexIndex->clear();
    ui.lineEditObjTexName->clear();
    ui.spinBoxObjTexIndex->clear();
    ui.checkBoxTileCanSteppedOn->setChecked(false);
    ui.checkBoxObjectAbovePlayer->setChecked(false);
    ui.checkBoxIsWallToClimb->setChecked(false);
    refreshEventList({});
}

void TilePropsComponent::refreshEventList(std::set<mapeditor::controllers::MapTileTriggerDTO> triggers) {
    ui.tableWidgetMapTileTriggers->model()->removeRows(0, ui.tableWidgetMapTileTriggers->rowCount());
    if (triggers.size() > 0) {
        int indexTrigger {0};
        for (const auto &trigger : triggers) {
            ui.tableWidgetMapTileTriggers->insertRow(indexTrigger);
            ui.tableWidgetMapTileTriggers->setItem(indexTrigger, 0,
                    new QTableWidgetItem(trigger.event.c_str()));
            indexTrigger++;
        }
    }
}

template <typename T, typename Getter, typename Setter>
void updateUIField(const std::vector<MapTileDTO> &tiles,
        T *uiField,
        const Getter& getter,
        const Setter& setter) {
    if (std::all_of(tiles.begin(), tiles.end(), [&](const MapTileDTO &tile) {
        return getter(tile) == getter(tiles.at(0));
    })) {
        setter(uiField, getter(tiles.at(0)), false);
    } else {
        setter(uiField, {}, true);
    }
}
void TilePropsComponent::onTileSelected(std::vector<MapTileDTO> tiles, Point<> coord) {
    if (tiles.size() == 0) {
        onTileUnselected();
        return;
    } else if (tiles.size() > 1) {
        ui.labelTileCoordXY->setText("X: <multi>, Y: <multi>");
    } else {
        ui.labelTileCoordXY->setText(fmt::format("X: {0}, Y: {1}", coord.x(), coord.y()).c_str());
    }

    m_disableFieldsChangedEvent = true;

    updateUIField(tiles, ui.lineEditTexName, [](const MapTileDTO &tile) { return tile.textureName; },
    [](QLineEdit *field, const std::string& value, bool) { field->setText(value.c_str()); });

    updateUIField(tiles, ui.spinBoxTexIndex, [](const MapTileDTO &tile) { return tile.textureIndex; },
    [](QSpinBox *field, int value, bool empty) { empty ? field->clear() : field->setValue(value); });

    updateUIField(tiles, ui.lineEditObjTexName, [](const MapTileDTO &tile) { return tile.objectTextureName; },
    [](QLineEdit *field, const std::string& value, bool) { field->setText(value.c_str()); });

    updateUIField(tiles, ui.spinBoxObjTexIndex, [](const MapTileDTO &tile) { return tile.objectTextureIndex; },
    [](QSpinBox *field, int value, bool empty) { empty ? field->clear() : field->setValue(value); });

    updateUIField(tiles, ui.checkBoxObjectAbovePlayer, [](const MapTileDTO &tile) { return tile.objectAbovePlayer; },
    [](QCheckBox *field, bool value, bool empty) { empty ? field->setChecked(false) : field->setChecked(value); });

    updateUIField(tiles, ui.checkBoxTileCanSteppedOn, [](const MapTileDTO &tile) { return tile.canSteppedOn; },
    [](QCheckBox *field, bool value, bool empty) { empty ? field->setChecked(false) : field->setChecked(value); });

    updateUIField(tiles, ui.checkBoxIsWallToClimb, [](const MapTileDTO &tile) { return tile.isWallToClimb; },
    [](QCheckBox *field, bool value, bool empty) { empty ? field->setChecked(false) : field->setChecked(value); });

    refreshEventList(m_controller.getTilesCommonTriggers());
    m_disableFieldsChangedEvent = false;
    setEnabledWidgetsInLayout(ui.verticalLayout_4, true);
}

void TilePropsComponent::onTileUnselected() {
    ui.labelTileCoordXY->clear();
    ui.lineEditTexName->clear();
    ui.spinBoxTexIndex->clear();
    ui.lineEditObjTexName->clear();
    ui.spinBoxObjTexIndex->clear();
    ui.checkBoxTileCanSteppedOn->setChecked(false);
    ui.checkBoxObjectAbovePlayer->setChecked(false);
    ui.checkBoxIsWallToClimb->setChecked(false);
    refreshEventList({});
    setEnabledWidgetsInLayout(ui.verticalLayout_4, false);
}

void setEnabledWidgetsInLayout(QLayout *layout, bool enabled) {
    if (layout == nullptr)
        return;

    QWidget *pw = layout->parentWidget();
    if (pw == nullptr)
        return;

    foreach(QWidget *w, pw->findChildren<QWidget*>()) {
        if (isChildWidgetOfAnyLayout(layout, w))
            w->setEnabled(enabled);
    }
}

bool isChildWidgetOfAnyLayout(QLayout *layout, QWidget *widget) {
    if (layout == nullptr || widget == nullptr)
        return false;

    if (layout->indexOf(widget) >= 0)
        return true;

    foreach(QObject *o, layout->children()) {
        if (isChildWidgetOfAnyLayout(qobject_cast<QLayout *>(o), widget))
            return true;
    }

    return false;
}

void TilePropsComponent::onLineEditTexNameTextChanged(const QString &text) {
    if (!m_disableFieldsChangedEvent) {
        m_controller.setTilesTextureName(text.toStdString());
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onSpinBoxTexIndexValueChanged(int value) {
    if (!m_disableFieldsChangedEvent) {
        m_controller.setTilesTextureIndex(value);
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onLineEditObjTexNameTextChanged(const QString &text) {
    if (!m_disableFieldsChangedEvent) {
        m_controller.setTilesObjectTextureName(text.toStdString());
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onSpinBoxObjTexIndexValueChanged(int value) {
    if (!m_disableFieldsChangedEvent) {
        m_controller.setTilesObjectTextureIndex(value);
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onCheckBoxObjectAbovePlayerChanged(int state) {
    if (!m_disableFieldsChangedEvent) {
        m_controller.setTilesObjectAbovePlayer(state == Qt::Checked);
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onCheckBoxTileCanSteppedOnChanged(int state) {
    if (!m_disableFieldsChangedEvent) {
        m_controller.setTilesCanSteppedOn(state == Qt::Checked);
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onCheckBoxIsWallToClimbChanged(int state) {
    if (!m_disableFieldsChangedEvent) {
        m_controller.setTilesIsWallToClimb(state == Qt::Checked);
        m_glComponent->updateGL();
    }
}

boost::optional<MapTileTriggerDTO> TilePropsComponent::getSelectedTrigger() {
    if (ui.tableWidgetMapTileTriggers->selectionModel()->hasSelection()) {
        // Find the selected trigger
        auto selectedItemName = ui.tableWidgetMapTileTriggers->selectionModel()->selectedRows()[0].data().toString().toStdString();
        return m_controller.findMapTileTriggerByEvent(selectedItemName);
    } else {
        return {};
    }
}

void TilePropsComponent::onPushButtonAddTileEventClick() {
    m_glComponent->stopAutoUpdate();
    auto selectedMapTiles = m_controller.getSelectedTiles();
    if (selectedMapTiles.size() > 0) {
        // Create a flat list of all tiles triggers
        std::vector<MapTileTriggerDTO> combinedTriggers = {};
        for (const auto &tile : selectedMapTiles) {
            combinedTriggers.insert(combinedTriggers.end(),
                    tile.triggers.begin(),
                    tile.triggers.end());
        }
        EditMapTileTriggerForm formEditMapTileTrigger(this,
                m_glComponent->getResourcesPath(),
                {},
                combinedTriggers);
        UIUtils::centerToScreen(&formEditMapTileTrigger);
        if (formEditMapTileTrigger.exec() == QDialog::Accepted) {
            m_controller.addTilesTrigger(formEditMapTileTrigger.getUpdatedTrigger());
            refreshEventList(m_controller.getTilesCommonTriggers());
        }
    }
    m_glComponent->startAutoUpdate();
}

void TilePropsComponent::onPushButtonEditTileEventClick() {
    m_glComponent->stopAutoUpdate();
    auto selectedMapTiles = m_controller.getSelectedTiles();
    if (selectedMapTiles.size() > 0) {
        // Create a flat list of all tiles triggers
        std::vector<MapTileTriggerDTO> combinedTriggers = {};
        for (const auto &tile : selectedMapTiles) {
            combinedTriggers.insert(combinedTriggers.end(),
                    tile.triggers.begin(),
                    tile.triggers.end());
        }
        //// Find the selected event
        auto selectedMapTileTrigger { getSelectedTrigger() };
        if (selectedMapTileTrigger.has_value()) {
            EditMapTileTriggerForm formEditMapTileTrigger(this,
                    m_glComponent->getResourcesPath(),
                    selectedMapTileTrigger,
                    combinedTriggers);
            UIUtils::centerToScreen(&formEditMapTileTrigger);
            if (formEditMapTileTrigger.exec() == QDialog::Accepted) {
                if (!m_controller.updateTilesTrigger(selectedMapTileTrigger.value(),
                        formEditMapTileTrigger.getUpdatedTrigger())) {
                    ErrorMessage::show("An error occurred while trying to update the selected trigger.\n"
                            "Some selected tiles may have been updated.");
                }
                refreshEventList(m_controller.getTilesCommonTriggers());
            }
        }
    }
    m_glComponent->startAutoUpdate();
}

void TilePropsComponent::onPushButtonDeleteTileEventClick() {
    m_glComponent->stopAutoUpdate();
    auto selectedMapTiles = m_controller.getSelectedTiles();
    if (selectedMapTiles.size() > 0) {
        // Create a flat list of all tiles triggers
        std::vector<MapTileTriggerDTO> combinedTriggers = {};
        for (const auto &tile : selectedMapTiles) {
            combinedTriggers.insert(combinedTriggers.end(),
                    tile.triggers.begin(),
                    tile.triggers.end());
        }
        //// Find the selected event
        auto selectedMapTileTrigger { getSelectedTrigger() };
        if (selectedMapTileTrigger.has_value()) {
            QMessageBox msgBox;
            msgBox.setText(fmt::format("Are you sure you want to delete the trigger {0}?",
                        selectedMapTileTrigger->event).c_str());
            msgBox.setWindowTitle("Confirmation");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            if (msgBox.exec() == QMessageBox::Yes) {
                if (!m_controller.deleteTilesTrigger(selectedMapTileTrigger.value())) {
                    ErrorMessage::show("An error occurred while trying to delete the selected trigger.\n"
                            "Some selected tiles may have been updated.");
                }
                refreshEventList(m_controller.getTilesCommonTriggers());
            }
        }
    }
    m_glComponent->startAutoUpdate();
}

void TilePropsComponent::onTableWidgetMapTileTriggersKeyPressEvent(int key, int, int) {
    if (key == Qt::Key_Delete) {
        onPushButtonDeleteTileEventClick();
    }
}
