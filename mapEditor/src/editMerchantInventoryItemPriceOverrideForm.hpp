
#ifndef MAPEDITOR_SRC_EDITMERCHANTINVENTORYITEMPRICEOVERRIDEFORM_HPP_
#define MAPEDITOR_SRC_EDITMERCHANTINVENTORYITEMPRICEOVERRIDEFORM_HPP_

#include <optional>
#include <string>
#include "itemPricingDTO.hpp"
#include "ui_editMerchantInventoryItemPriceOverrideForm.h"

class EditMerchantInventoryItemPriceOverrideForm : public QDialog {
Q_OBJECT

 public:     
    struct ItemPricingResult {
      std::optional<unsigned int> buyPriceOverride;
      std::optional<unsigned int> sellPriceOverride;
    };
    EditMerchantInventoryItemPriceOverrideForm(QWidget *parent, const mapeditor::controllers::ItemPricingDTO &selectedItemPricing);
    const ItemPricingResult &getResult() const;
    void connectUIActions();
 private:
    Ui::editMerchantInventoryItemPriceOverrideFormClass ui;
    ItemPricingResult m_result;
    void onPushButtonOKClick();
    void onCheckBoxOverrideBuyPriceChanged(int state);
    void onCheckBoxOverrideSellPriceChanged(int state);
    void toggleBuyPriceElements();
    void toggleSellPriceElement();
};

#endif  // MAPEDITOR_SRC_EDITMERCHANTINVENTORYITEMPRICEOVERRIDEFORM_HPP_
