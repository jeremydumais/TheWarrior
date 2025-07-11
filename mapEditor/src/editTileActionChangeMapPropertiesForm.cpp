#include "editTileActionChangeMapPropertiesForm.hpp"
#include <fmt/format.h>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <algorithm>
#include "errorMessage.hpp"

using commoneditor::ui::ErrorMessage;

EditTileActionChangeMapPropertiesForm::EditTileActionChangeMapPropertiesForm(QWidget *parent,
        const std::string &resourcesPath,
        const std::map<std::string, std::string> &properties)
    : QDialog(parent),
    ui(Ui::editTileActionChangeMapPropertiesFormClass()),
    m_resourcesPath(resourcesPath),
    m_properties(properties) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditTileActionChangeMapPropertiesForm::onPushButtonOK);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditTileActionChangeMapPropertiesForm::reject);
    connect(ui.pushButtonOpenMapFile,
            &QPushButton::clicked,
            this,
            &EditTileActionChangeMapPropertiesForm::onPushButtonOpenMapFileClick);

    ui.comboBoxPlayerFacing->model()->removeRows(0, ui.comboBoxPlayerFacing->count());
    ui.comboBoxPlayerFacing->insertItem(0, "Up");
    ui.comboBoxPlayerFacing->insertItem(1, "Down");
    ui.comboBoxPlayerFacing->insertItem(2, "Left");
    ui.comboBoxPlayerFacing->insertItem(3, "Right");

    if (this->m_properties.find("mapFileName") != this->m_properties.end()) {
        ui.lineEditMapFileName->setText(this->m_properties.at("mapFileName").c_str());
    }
    if (this->m_properties.find("playerX") != this->m_properties.end()) {
        ui.spinBoxPlayerX->setValue(stoi(this->m_properties.at("playerX")));
    }
    if (this->m_properties.find("playerY") != this->m_properties.end()) {
        ui.spinBoxPlayerY->setValue(stoi(this->m_properties.at("playerY")));
    }
    if (this->m_properties.find("playerFacing") != this->m_properties.end()) {
        ui.comboBoxPlayerFacing->setCurrentIndex(stoi(this->m_properties.at("playerFacing")));
    }
}

const std::map<std::string, std::string> &EditTileActionChangeMapPropertiesForm::getUpdatedProperties() const {
    return m_properties;
}

void EditTileActionChangeMapPropertiesForm::onPushButtonOK() {
    if (ui.lineEditMapFileName->text().isEmpty()) {
        ErrorMessage::show("The map file cannot be empty.");
        return;
    }

    m_properties.clear();
    m_properties["mapFileName"] = ui.lineEditMapFileName->text().toStdString();
    m_properties["playerX"] = std::to_string(ui.spinBoxPlayerX->value());
    m_properties["playerY"] = std::to_string(ui.spinBoxPlayerY->value());
    m_properties["playerFacing"] = std::to_string(ui.comboBoxPlayerFacing->currentIndex());
    accept();
}

void EditTileActionChangeMapPropertiesForm::onPushButtonOpenMapFileClick() {
    QString fullFilePath { QFileDialog::getOpenFileName(this,
            tr("Open Map"),
            m_resourcesPath.c_str(),
            tr("Map file (*.map)")) };
    QFileInfo fileInfo(fullFilePath);
    std::string filename { fileInfo.fileName().toStdString() };
    ui.lineEditMapFileName->setText(filename.c_str());
}
