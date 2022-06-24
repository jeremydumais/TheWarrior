#include "playerEquipment.hpp"
#include <gtest/gtest.h>

/*class PlayerEquipmentSample1 : public ::testing::Test
{
public:
    PlayerEquipmentSample1():
        pla
    PlayerEquipment playerEquipment;
};
*/
TEST(PlayerEquipment_DefaultConstructor, ReturnEmptyPlayerEquipment)
{
    PlayerEquipment playerEquipment;
    ASSERT_EQ(boost::none, playerEquipment.getMainHand());
    ASSERT_EQ(boost::none, playerEquipment.getSecondaryHand());
}
