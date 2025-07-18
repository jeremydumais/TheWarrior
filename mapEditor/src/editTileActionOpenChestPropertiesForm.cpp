#include "editTileActionOpenChestPropertiesForm.hpp"
#include <fmt/format.h>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <algorithm>

EditTileActionOpenChestPropertiesForm::EditTileActionOpenChestPropertiesForm(QWidget *parent,
        const std::map<std::string, std::string> &properties)
    : QDialog(parent),
    ui(Ui::editTileActionOpenChestPropertiesFormClass()),
    m_properties(properties) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditTileActionOpenChestPropertiesForm::onPushButtonOK);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditTileActionOpenChestPropertiesForm::reject);

    if (this->m_properties.find("objectTextureIndexOpenedChest") != this->m_properties.end()) {
        ui.spinBoxObjectTextureIndexOpenedChest->setValue(stoi(this->m_properties.at("objectTextureIndexOpenedChest")));
    }
    if (this->m_properties.find("itemIdInside") != this->m_properties.end()) {
        ui.lineEditItemIdInside->setText(this->m_properties.at("itemIdInside").c_str());
    }
}

const std::map<std::string, std::string> &EditTileActionOpenChestPropertiesForm::getUpdatedProperties() const {
    return m_properties;
}

void EditTileActionOpenChestPropertiesForm::onPushButtonOK() {
    m_properties.clear();
    m_properties["objectTextureIndexOpenedChest"] = std::to_string(ui.spinBoxObjectTextureIndexOpenedChest->value());
    m_properties["itemIdInside"] = ui.lineEditItemIdInside->text().toStdString();
    accept();
}
