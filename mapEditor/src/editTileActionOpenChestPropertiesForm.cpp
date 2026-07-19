#include "editTileActionOpenChestPropertiesForm.hpp"
#include <fmt/format.h>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <algorithm>
#include "errorMessage.hpp"
#include "openChestActionPropertiesConverter.hpp"

using commoneditor::ui::ErrorMessage;
using thewarrior::models::ChestContentType;
using thewarrior::models::OpenChestActionProperties;
using thewarrior::models::OpenChestActionPropertiesConverter;

EditTileActionOpenChestPropertiesForm::EditTileActionOpenChestPropertiesForm(QWidget *parent,
        const std::map<std::string, std::string> &properties)
    : QDialog(parent),
    ui(Ui::editTileActionOpenChestPropertiesFormClass()),
    m_properties(properties) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditTileActionOpenChestPropertiesForm::onPushButtonOK);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditTileActionOpenChestPropertiesForm::reject);

    auto props = OpenChestActionPropertiesConverter::fromMap(m_properties);
    ui.spinBoxObjectTextureIndexOpenedChest->setValue(props.getOpenedTextureIndex());
    if (props.getContentType() == ChestContentType::Item) {
        ui.radioButtonItem->setChecked(true);
        ui.lineEditItemIdInside->setText(props.getItemId().c_str());
    } else if (props.getContentType() == ChestContentType::Gold) {
        ui.radioButtonGold->setChecked(true);
        ui.spinBoxGoldAmount->setValue(static_cast<int>(props.getGoldAmount()));
    } else {
        ui.radioButtonNothing->setChecked(true);
    }
}

const std::map<std::string, std::string> &EditTileActionOpenChestPropertiesForm::getUpdatedProperties() const {
    return m_properties;
}

void EditTileActionOpenChestPropertiesForm::onPushButtonOK() {
    int openedTextureIndex = ui.spinBoxObjectTextureIndexOpenedChest->value();
    if (openedTextureIndex <= -1) {
        ErrorMessage::show("The Object Texture Index for opened chest must have a positive value.");
        return;
    }

    m_properties.clear();
    OpenChestActionProperties props;
    props.setOpenedTextureIndex(openedTextureIndex);

    if (ui.radioButtonItem->isChecked()) {
        props.setContentType(ChestContentType::Item);
        props.setItemId(ui.lineEditItemIdInside->text().toStdString());
    } else if (ui.radioButtonGold->isChecked()) {
        props.setContentType(ChestContentType::Gold);
        props.setGoldAmount(ui.spinBoxGoldAmount->value());
    } else {
        props.setContentType(ChestContentType::Nothing);
    }
    m_properties = OpenChestActionPropertiesConverter::toMap(props);

    accept();
}
