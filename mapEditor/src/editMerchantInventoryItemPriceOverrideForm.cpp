#include <qstring.h>
#include "editMerchantInventoryItemPriceOverrideForm.hpp"
#include "itemPricingDTO.hpp"

using mapeditor::controllers::ItemPricingDTO;

EditMerchantInventoryItemPriceOverrideForm::EditMerchantInventoryItemPriceOverrideForm(QWidget *parent, 
                                                                                       const ItemPricingDTO &selectedItemPricing)
: QDialog(parent),
  ui(Ui::editMerchantInventoryItemPriceOverrideFormClass()) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size()); 
    ui.labelItemId->setText(selectedItemPricing.itemId.c_str());
    ui.labelDefaultBuyPriceValue->setText(QString::number(selectedItemPricing.defaultBuyPrice));
    ui.labelDefaultSellPriceValue->setText(QString::number(selectedItemPricing.defaultSellPrice));
    if (selectedItemPricing.buyPriceOverride.has_value()) {
        ui.spinBoxBuyPrice->setValue(static_cast<int>(selectedItemPricing.buyPriceOverride.value()));
        ui.spinBoxBuyPrice->setVisible(true);
        ui.checkBoxOverrideBuyPrice->setChecked(true);
    }
    if (selectedItemPricing.sellPriceOverride.has_value()) {
        ui.spinBoxSellPrice->setValue(static_cast<int>(selectedItemPricing.sellPriceOverride.value()));
        ui.spinBoxSellPrice->setVisible(true);
        ui.checkBoxOverrideSellPrice->setChecked(true);
    }
    connectUIActions();
    toggleBuyPriceElements();
    toggleSellPriceElement();
}

const EditMerchantInventoryItemPriceOverrideForm::ItemPricingResult &EditMerchantInventoryItemPriceOverrideForm::getResult() const {
    return m_result;
}

void EditMerchantInventoryItemPriceOverrideForm::connectUIActions() {
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditMerchantInventoryItemPriceOverrideForm::onPushButtonOKClick);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditMerchantInventoryItemPriceOverrideForm::reject);
    connect(ui.checkBoxOverrideBuyPrice, &QCheckBox::stateChanged, this, &EditMerchantInventoryItemPriceOverrideForm::onCheckBoxOverrideBuyPriceChanged);
    connect(ui.checkBoxOverrideSellPrice, &QCheckBox::stateChanged, this, &EditMerchantInventoryItemPriceOverrideForm::onCheckBoxOverrideSellPriceChanged);
}

void EditMerchantInventoryItemPriceOverrideForm::onPushButtonOKClick() {
    if (ui.checkBoxOverrideBuyPrice->isChecked()) {
        m_result.buyPriceOverride = static_cast<unsigned int>(ui.spinBoxBuyPrice->value());
    } else {
        m_result.buyPriceOverride.reset();
    }

    if (ui.checkBoxOverrideSellPrice->isChecked()) {
        m_result.sellPriceOverride = static_cast<unsigned int>(ui.spinBoxSellPrice->value());
    } else {
        m_result.sellPriceOverride.reset();
    }
    accept();
}

void EditMerchantInventoryItemPriceOverrideForm::onCheckBoxOverrideBuyPriceChanged(int /*state*/) {
    toggleBuyPriceElements();
}

void EditMerchantInventoryItemPriceOverrideForm::onCheckBoxOverrideSellPriceChanged(int /*state*/) {
    toggleSellPriceElement();
}

void EditMerchantInventoryItemPriceOverrideForm::toggleBuyPriceElements() {
    ui.spinBoxBuyPrice->setVisible(ui.checkBoxOverrideBuyPrice->isChecked());
}

void EditMerchantInventoryItemPriceOverrideForm::toggleSellPriceElement() {
    ui.spinBoxSellPrice->setVisible(ui.checkBoxOverrideSellPrice->isChecked());
}