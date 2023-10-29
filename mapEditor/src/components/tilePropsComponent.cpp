#include <fmt/format.h>
#include "mapTile.hpp"
#include "tilePropsComponent.hpp"
#include "editMapTileTriggerForm.hpp"
#include "errorMessage.hpp"
#include "mapTileTriggerEventConverter.hpp"
#include "uiUtils.hpp"

using commoneditor::ui::ErrorMessage;
using commoneditor::ui::UIUtils;
using thewarrior::models::MapTile;
using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerEventConverter;
using thewarrior::models::Point;

TilePropsComponent::TilePropsComponent(QWidget *parent,
        MainForm_GLComponent *glComponent)
    : QWidget(parent),
      ui(Ui::TilePropsComponent()),
      m_glComponent(glComponent) {
      ui.setupUi(this);
      connectUIActions();
}

void TilePropsComponent::connectUIActions() {
    connect(m_glComponent, &MainForm_GLComponent::tileSelected, this, &TilePropsComponent::onTileSelected);
    connect(ui.lineEditTexName, &QLineEdit::textChanged, this, &TilePropsComponent::onLineEditTexNameTextChanged);
    connect(ui.spinBoxTexIndex, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &TilePropsComponent::onSpinBoxTexIndexValueChanged);
    connect(ui.lineEditObjTexName, &QLineEdit::textChanged, this, &TilePropsComponent::onLineEditObjTexNameTextChanged);
    connect(ui.spinBoxObjTexIndex, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &TilePropsComponent::onSpinBoxObjTexIndexValueChanged);
    connect(ui.checkBoxTileCanSteppedOn, &QCheckBox::stateChanged, this, &TilePropsComponent::onCheckBoxTileCanSteppedOnChanged);
    connect(ui.checkBoxObjectAbovePlayer, &QCheckBox::stateChanged, this, &TilePropsComponent::onCheckBoxObjectAbovePlayerChanged);
    connect(ui.checkBoxIsWallToClimb, &QCheckBox::stateChanged, this, &TilePropsComponent::onCheckBoxIsWallToClimbChanged);
    connect(ui.pushButtonAddTileEvent, &QPushButton::clicked, this, &TilePropsComponent::onPushButtonAddTileEventClick);
    connect(ui.pushButtonEditTileEvent, &QPushButton::clicked, this, &TilePropsComponent::onPushButtonEditTileEventClick);
    connect(ui.pushButtonDeleteTileEvent, &QPushButton::clicked, this, &TilePropsComponent::onPushButtonDeleteTileEventClick);
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
    ui.listWidgetMapTileTriggers->model()->removeRows(0, ui.listWidgetMapTileTriggers->count());
    if (tile != nullptr) {
        int indexTrigger {0};
        for (const auto &trigger : tile->getTriggers()) {
            ui.listWidgetMapTileTriggers->insertItem(indexTrigger, MapTileTriggerEventConverter::eventToString(trigger.getEvent()).c_str());
            indexTrigger++;
        }
    }
}

void TilePropsComponent::onTileSelected(MapTile *tile, Point<> coord) {
    ui.labelTileCoordXY->setText(fmt::format("X: {0}, Y: {1}", coord.x(), coord.y()).c_str());
    ui.lineEditTexName->setText(tile->getTextureName().c_str());
    ui.spinBoxTexIndex->setValue(tile->getTextureIndex());
    ui.lineEditObjTexName->setText(tile->getObjectTextureName().c_str());
    ui.spinBoxObjTexIndex->setValue(tile->getObjectTextureIndex());
    ui.checkBoxTileCanSteppedOn->setChecked(tile->canPlayerSteppedOn());
    ui.checkBoxObjectAbovePlayer->setChecked(tile->getObjectAbovePlayer());
    ui.checkBoxIsWallToClimb->setChecked(tile->getIsWallToClimb());
    refreshEventList(tile);
}


void TilePropsComponent::onLineEditTexNameTextChanged(const QString &text) {
    auto currentMapTile = m_glComponent->getCurrentMapTile();
    if (currentMapTile != nullptr) {
        currentMapTile->setTextureName(text.toStdString());
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onSpinBoxTexIndexValueChanged(int value) {
    auto currentMapTile = m_glComponent->getCurrentMapTile();
    if (currentMapTile != nullptr) {
        currentMapTile->setTextureIndex(value);
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onLineEditObjTexNameTextChanged(const QString &text) {
    auto currentMapTile = m_glComponent->getCurrentMapTile();
    if (currentMapTile != nullptr) {
        currentMapTile->setObjectTextureName(text.toStdString());
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onSpinBoxObjTexIndexValueChanged(int value) {
    auto currentMapTile = m_glComponent->getCurrentMapTile();
    if (currentMapTile != nullptr) {
        currentMapTile->setObjectTextureIndex(value);
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onCheckBoxObjectAbovePlayerChanged(int state) {
    auto currentMapTile = m_glComponent->getCurrentMapTile();
    if (currentMapTile != nullptr) {
        currentMapTile->setObjectAbovePlayer(state == Qt::Checked);
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onCheckBoxTileCanSteppedOnChanged(int state) {
    auto currentMapTile = m_glComponent->getCurrentMapTile();
    if (currentMapTile != nullptr) {
        currentMapTile->setCanPlayerSteppedOn(state == Qt::Checked);
        m_glComponent->updateGL();
    }
}

void TilePropsComponent::onCheckBoxIsWallToClimbChanged(int state) {
    auto currentMapTile = m_glComponent->getCurrentMapTile();
    if (currentMapTile != nullptr) {
        currentMapTile->setIsWallToClimb(state == Qt::Checked);
        m_glComponent->updateGL();
    }
}

boost::optional<MapTileTrigger &> TilePropsComponent::getSelectedTrigger() {
    auto currentMapTile = m_glComponent->getCurrentMapTile();
    if (ui.listWidgetMapTileTriggers->selectionModel()->hasSelection()) {
        // Find the selected trigger
        auto selectedItemName { ui.listWidgetMapTileTriggers->selectionModel()->selectedRows()[0].data().toString().toStdString() };
        auto parsedEvent { MapTileTriggerEventConverter::eventFromString(selectedItemName) };
        if (parsedEvent.has_value()) {
            return currentMapTile->findTrigger(parsedEvent.get());
        } else {
            return {};
        }
    } else {
        return {};
    }
}

void TilePropsComponent::onPushButtonAddTileEventClick() {
    m_glComponent->stopAutoUpdate();
    auto currentMapTile = m_glComponent->getCurrentMapTile();
    if (currentMapTile != nullptr) {
        EditMapTileTriggerForm formEditMapTileTrigger(this,
                m_glComponent->getResourcesPath(),
                nullptr,
                currentMapTile->getTriggers());
        UIUtils::centerToScreen(&formEditMapTileTrigger);
        if (formEditMapTileTrigger.exec() == QDialog::Accepted) {
            currentMapTile->addTrigger(formEditMapTileTrigger.getUpdatedTrigger());
            refreshEventList(m_glComponent->getCurrentMapTile());
        }
    }
    m_glComponent->startAutoUpdate();
}

void TilePropsComponent::onPushButtonEditTileEventClick() {
    m_glComponent->stopAutoUpdate();
    auto currentMapTile = m_glComponent->getCurrentMapTile();
    if (currentMapTile != nullptr) {
        // Find the selected event
        auto selectedMapTileTrigger { getSelectedTrigger() };
        if (selectedMapTileTrigger.has_value()) {
            EditMapTileTriggerForm formEditMapTileTrigger(this,
                    m_glComponent->getResourcesPath(),
                    &selectedMapTileTrigger.get(),
                    currentMapTile->getTriggers());
            UIUtils::centerToScreen(&formEditMapTileTrigger);
            if (formEditMapTileTrigger.exec() == QDialog::Accepted) {
                if (!currentMapTile->updateTrigger(selectedMapTileTrigger.get(), formEditMapTileTrigger.getUpdatedTrigger())) {
                    ErrorMessage::show("An error occurred while trying to update the selected trigger.");
                }
                refreshEventList(m_glComponent->getCurrentMapTile());
            }
        }
    }
    m_glComponent->startAutoUpdate();
}

void TilePropsComponent::onPushButtonDeleteTileEventClick() {
    m_glComponent->stopAutoUpdate();
    auto currentMapTile = m_glComponent->getCurrentMapTile();
    if (currentMapTile != nullptr) {
        // Find the selected event
        auto selectedMapTileTrigger { getSelectedTrigger() };
        if (selectedMapTileTrigger.has_value()) {
            QMessageBox msgBox;
            msgBox.setText(fmt::format("Are you sure you want to delete the trigger {0}?",
                        MapTileTriggerEventConverter::eventToString(selectedMapTileTrigger->getEvent())).c_str());
            msgBox.setWindowTitle("Confirmation");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            if (msgBox.exec() == QMessageBox::Yes) {
                currentMapTile->deleteTrigger(selectedMapTileTrigger.get());
                refreshEventList(m_glComponent->getCurrentMapTile());
            }
        }
    }
    m_glComponent->startAutoUpdate();
}
