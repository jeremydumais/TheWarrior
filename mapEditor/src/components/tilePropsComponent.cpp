#include <fmt/format.h>
#include <qtablewidget.h>
#include <algorithm>
#include <string>
#include "glComponentController.hpp"
#include "mapTile.hpp"
#include "mapTileDTO.hpp"
#include "tilePropsComponent.hpp"
#include "editMapTileTriggerForm.hpp"
#include "errorMessage.hpp"
#include "mapTileTriggerEventConverter.hpp"
#include "uiUtils.hpp"

using commoneditor::ui::ErrorMessage;
using commoneditor::ui::UIUtils;
using mapeditor::controllers::GLComponentController;
using mapeditor::controllers::MapTileDTO;
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
    refreshEventList(nullptr);
}

void TilePropsComponent::refreshEventList(MapTile *tile) {
    ui.tableWidgetMapTileTriggers->model()->removeRows(0, ui.tableWidgetMapTileTriggers->rowCount());
    if (tile != nullptr) {
        int indexTrigger {0};
        for (const auto &trigger : tile->getTriggers()) {
            ui.tableWidgetMapTileTriggers->insertRow(indexTrigger);
            ui.tableWidgetMapTileTriggers->setItem(indexTrigger, 0,
                    new QTableWidgetItem(MapTileTriggerEventConverter::eventToString(trigger.getEvent()).c_str()));
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
    // TODO: 0.3.3 To solve
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

    //refreshEventList(tile);
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
    refreshEventList(nullptr);
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
        auto tiles = m_glComponent->getCurrentMapTiles();
        std::for_each(tiles.begin(), tiles.end(), [&text](const MapTileDTO &tile) {
                //tile->setTextureName(text.toStdString());
                });
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onSpinBoxTexIndexValueChanged(int value) {
    if (!m_disableFieldsChangedEvent) {
        auto tiles = m_glComponent->getCurrentMapTiles();
        std::for_each(tiles.begin(), tiles.end(), [&value](const MapTileDTO tile) {
                //tile->setTextureIndex(value);
                });
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onLineEditObjTexNameTextChanged(const QString &text) {
    if (!m_disableFieldsChangedEvent) {
        auto tiles = m_glComponent->getCurrentMapTiles();
        std::for_each(tiles.begin(), tiles.end(), [&text](const MapTileDTO &tile) {
                //tile->setObjectTextureName(text.toStdString());
                });
    m_glComponent->updateGL();
    }
}

void TilePropsComponent::onSpinBoxObjTexIndexValueChanged(int value) {
    if (!m_disableFieldsChangedEvent) {
        auto tiles = m_glComponent->getCurrentMapTiles();
        std::for_each(tiles.begin(), tiles.end(), [&value](const MapTileDTO &tile) {
                //tile->setObjectTextureIndex(value);
                });
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onCheckBoxObjectAbovePlayerChanged(int state) {
    if (!m_disableFieldsChangedEvent) {
        auto tiles = m_glComponent->getCurrentMapTiles();
        std::for_each(tiles.begin(), tiles.end(), [&state](const MapTileDTO &tile) {
                //tile->setObjectAbovePlayer(state == Qt::Checked);
                });
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onCheckBoxTileCanSteppedOnChanged(int state) {
    if (!m_disableFieldsChangedEvent) {
        auto tiles = m_glComponent->getCurrentMapTiles();
        std::for_each(tiles.begin(), tiles.end(), [&state](const MapTileDTO &tile) {
                //tile->setCanPlayerSteppedOn(state == Qt::Checked);
                });
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onCheckBoxIsWallToClimbChanged(int state) {
    if (!m_disableFieldsChangedEvent) {
        auto tiles = m_glComponent->getCurrentMapTiles();
        std::for_each(tiles.begin(), tiles.end(), [&state](const MapTileDTO &tile) {
                //tile->setIsWallToClimb(state == Qt::Checked);
        });
        m_glComponent->updateGL();
    }
}

boost::optional<MapTileTrigger &> TilePropsComponent::getSelectedTrigger() {
    // TODO: 0.3.3 To solve
    //auto currentMapTile = m_glComponent->getCurrentMapTile();
    //if (ui.tableWidgetMapTileTriggers->selectionModel()->hasSelection()) {
        //// Find the selected trigger
        //auto selectedItemName { ui.tableWidgetMapTileTriggers->selectionModel()->selectedRows()[0].data().toString().toStdString() };
        //auto parsedEvent { MapTileTriggerEventConverter::eventFromString(selectedItemName) };
        //if (parsedEvent.has_value()) {
            //return currentMapTile->findTrigger(parsedEvent.get());
        //} else {
            //return {};
        //}
    //} else {
        return {};
    //}
}

void TilePropsComponent::onPushButtonAddTileEventClick() {
    // TODO: 0.3.3 To solve
    //m_glComponent->stopAutoUpdate();
    //auto currentMapTile = m_glComponent->getCurrentMapTile();
    //if (currentMapTile != nullptr) {
        //EditMapTileTriggerForm formEditMapTileTrigger(this,
                //m_glComponent->getResourcesPath(),
                //nullptr,
                //currentMapTile->getTriggers());
        //UIUtils::centerToScreen(&formEditMapTileTrigger);
        //if (formEditMapTileTrigger.exec() == QDialog::Accepted) {
            //currentMapTile->addTrigger(formEditMapTileTrigger.getUpdatedTrigger());
            //refreshEventList(m_glComponent->getCurrentMapTile());
        //}
    //}
    //m_glComponent->startAutoUpdate();
}

void TilePropsComponent::onPushButtonEditTileEventClick() {
    // TODO: 0.3.3 To solve
    //m_glComponent->stopAutoUpdate();
    //auto currentMapTile = m_glComponent->getCurrentMapTile();
    //if (currentMapTile != nullptr) {
        //// Find the selected event
        //auto selectedMapTileTrigger { getSelectedTrigger() };
        //if (selectedMapTileTrigger.has_value()) {
            //EditMapTileTriggerForm formEditMapTileTrigger(this,
                    //m_glComponent->getResourcesPath(),
                    //&selectedMapTileTrigger.get(),
                    //currentMapTile->getTriggers());
            //UIUtils::centerToScreen(&formEditMapTileTrigger);
            //if (formEditMapTileTrigger.exec() == QDialog::Accepted) {
                //if (!currentMapTile->updateTrigger(selectedMapTileTrigger.get(), formEditMapTileTrigger.getUpdatedTrigger())) {
                    //ErrorMessage::show("An error occurred while trying to update the selected trigger.");
                //}
                //refreshEventList(m_glComponent->getCurrentMapTile());
            //}
        //}
    //}
    //m_glComponent->startAutoUpdate();
}

void TilePropsComponent::onPushButtonDeleteTileEventClick() {
    // TODO: 0.3.3 To solve
    //m_glComponent->stopAutoUpdate();
    //auto currentMapTile = m_glComponent->getCurrentMapTile();
    //if (currentMapTile != nullptr) {
        //// Find the selected event
        //auto selectedMapTileTrigger { getSelectedTrigger() };
        //if (selectedMapTileTrigger.has_value()) {
            //QMessageBox msgBox;
            //msgBox.setText(fmt::format("Are you sure you want to delete the trigger {0}?",
                        //MapTileTriggerEventConverter::eventToString(selectedMapTileTrigger->getEvent())).c_str());
            //msgBox.setWindowTitle("Confirmation");
            //msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            //msgBox.setDefaultButton(QMessageBox::Cancel);
            //if (msgBox.exec() == QMessageBox::Yes) {
                //currentMapTile->deleteTrigger(selectedMapTileTrigger.get());
                //refreshEventList(m_glComponent->getCurrentMapTile());
            //}
        //}
    //}
    //m_glComponent->startAutoUpdate();
}

void TilePropsComponent::onTableWidgetMapTileTriggersKeyPressEvent(int key, int, int) {
    if (key == Qt::Key_Delete) {
        onPushButtonDeleteTileEventClick();
    }
}
