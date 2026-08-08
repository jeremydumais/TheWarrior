#include <gtest/gtest.h>
#include "editMerchantInventoryFormController.hpp"

namespace mapeditor::controllers::editmerchantzoneformcontroller::unittest {

class EditMerchantInventoryFormControllerNameValidationTest : public ::testing::Test {
 protected:
    std::shared_ptr<EditMerchantInventoryFormController> makeController(
        const std::optional<MerchantInventoryDTO> &selectedMerchantInventory = std::nullopt) {
        return std::make_shared<EditMerchantInventoryFormController>(
            std::make_shared<ContainerOfItemStore>(),
            "fakeResourcesPath",
            thewarrior::models::MerchantInventoryType::WeaponsAndArmors,
            selectedMerchantInventory,
            std::vector<std::string> { "General Store", "Armory" });
    }
};

TEST_F(EditMerchantInventoryFormControllerNameValidationTest, NewInventory_WithUsedName_ReturnTrue) {
    ASSERT_TRUE(makeController()->isMerchantInventoryNameAlreadyUsed("General Store"));
}

TEST_F(EditMerchantInventoryFormControllerNameValidationTest, NewInventory_WithUsedNameDifferentCaseAndWhitespace_ReturnTrue) {
    ASSERT_TRUE(makeController()->isMerchantInventoryNameAlreadyUsed("  GENERAL STORE  "));
}

TEST_F(EditMerchantInventoryFormControllerNameValidationTest, NewInventory_WithUnusedName_ReturnFalse) {
    ASSERT_FALSE(makeController()->isMerchantInventoryNameAlreadyUsed("Potion Shop"));
}

TEST_F(EditMerchantInventoryFormControllerNameValidationTest, ExistingInventory_WithItsCurrentName_ReturnFalse) {
    const MerchantInventoryDTO selectedInventory {
        .name = "General Store",
        .inventoryType = thewarrior::models::MerchantInventoryType::WeaponsAndArmors,
        .items = {}
    };

    ASSERT_FALSE(makeController(selectedInventory)->isMerchantInventoryNameAlreadyUsed(" general STORE "));
}

TEST_F(EditMerchantInventoryFormControllerNameValidationTest, ExistingInventory_WithAnotherUsedName_ReturnTrue) {
    const MerchantInventoryDTO selectedInventory {
        .name = "General Store",
        .inventoryType = thewarrior::models::MerchantInventoryType::WeaponsAndArmors,
        .items = {}
    };

    ASSERT_TRUE(makeController(selectedInventory)->isMerchantInventoryNameAlreadyUsed("Armory"));
}

TEST_F(EditMerchantInventoryFormControllerNameValidationTest, ExistingInventory_WithUnusedName_ReturnFalse) {
    const MerchantInventoryDTO selectedInventory {
        .name = "General Store",
        .inventoryType = thewarrior::models::MerchantInventoryType::WeaponsAndArmors,
        .items = {}
    };

    ASSERT_FALSE(makeController(selectedInventory)->isMerchantInventoryNameAlreadyUsed("Potion Shop"));
}

}  // namespace mapeditor::controllers::editmerchantzoneformcontroller::unittest
