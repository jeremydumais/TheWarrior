#include "mapTile.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace thewarrior::models;

class MapTileWith2Triggers : public ::testing::Test
{
public:
	MapTileWith2Triggers()
	  : firstTrigger(MapTileTrigger(MapTileTriggerEvent::MoveUpPressed,
								   	MapTileTriggerCondition::None,
								    MapTileTriggerAction::OpenChest,
								    map<string, string>())),
		secondTrigger(MapTileTrigger(MapTileTriggerEvent::MoveDownPressed,
								   	 MapTileTriggerCondition::None,
								     MapTileTriggerAction::ChangeMap,
								     map<string, string>()))
	{
		tile.addTrigger(firstTrigger);
		tile.addTrigger(secondTrigger);
	}
	MapTile tile;
	MapTileTrigger firstTrigger;
	MapTileTrigger secondTrigger;
};

TEST(MapTile_Constructor, DefaultConstructor_ReturnEmptyTile)
{
	MapTile tile;
	ASSERT_EQ("", tile.getTextureName());
	ASSERT_EQ(-1, tile.getTextureIndex());
	ASSERT_EQ("", tile.getObjectTextureName());
	ASSERT_EQ(-1, tile.getObjectTextureIndex());
}

TEST(MapTile_getTextureName, DefaultConstructor_ReturnEmptyString)
{
	MapTile tile;
	ASSERT_EQ("", tile.getTextureName());
}

TEST(MapTile_getTextureName, withNameABC_ReturnABC)
{
	MapTile tile;
	tile.setTextureName("ABC");
	ASSERT_EQ("ABC", tile.getTextureName());
}

TEST(MapTile_getTextureIndex, DefaultConstructor_ReturnMinus1)
{
	MapTile tile;
	ASSERT_EQ(-1, tile.getTextureIndex());
}

TEST(MapTile_getTextureIndex, With1_Return1)
{
	MapTile tile;
	tile.setTextureIndex(1);
	ASSERT_EQ(1, tile.getTextureIndex());
}

TEST(MapTile_getObjectTextureName, DefaultConstructor_ReturnEmptyString)
{
	MapTile tile;
	ASSERT_EQ("", tile.getObjectTextureName());
}

TEST(MapTile_getObjectTextureName, withNameABC_ReturnABC)
{
	MapTile tile;
	tile.setObjectTextureName("ABC");
	ASSERT_EQ("ABC", tile.getObjectTextureName());
}

TEST(MapTile_getObjectTextureIndex, DefaultConstructor_ReturnMinus1)
{
	MapTile tile;
	ASSERT_EQ(-1, tile.getObjectTextureIndex());
}

TEST(MapTile_getObjectTextureIndex, With1_Return1)
{
	MapTile tile;
	tile.setObjectTextureIndex(1);
	ASSERT_EQ(1, tile.getObjectTextureIndex());
}

TEST(MapTile_isAssigned, WithAllFieldsEmpty_ReturnFalse)
{
	MapTile tile;
	ASSERT_FALSE(tile.isAssigned());
}

TEST(MapTile_isAssigned, WithOnlyTextureNameNotEmpty_ReturnTrue)
{
	MapTile tile;
	tile.setTextureName("Tex1");
	ASSERT_TRUE(tile.isAssigned());
}

TEST(MapTile_isAssigned, WithOnlyTextureIndex0_ReturnTrue)
{
	MapTile tile;
	tile.setTextureIndex(0);
	ASSERT_TRUE(tile.isAssigned());
}

TEST(MapTile_isAssigned, WithOnlyObjectTextureNameNotEmpty_ReturnTrue)
{
	MapTile tile;
	tile.setObjectTextureName("Tex1");
	ASSERT_TRUE(tile.isAssigned());
}

TEST(MapTile_isAssigned, WithOnlyObjectTextureIndex0_ReturnTrue)
{
	MapTile tile;
	tile.setObjectTextureIndex(0);
	ASSERT_TRUE(tile.isAssigned());
}

TEST(MapTile_setTextureName, WithABC_ReturnSuccess)
{
	MapTile tile;
	tile.setTextureName("ABC");
	ASSERT_EQ("ABC", tile.getTextureName());
}

TEST(MapTile_isAssigned, WithTextureNameAndIndexNotEmpty_ReturnTrue)
{
	MapTile tile;
	tile.setTextureName("Tex1");
	tile.setTextureIndex(0);
	ASSERT_TRUE(tile.isAssigned());
}

TEST(MapTile_isAssigned, WithAllFieldsNotEmpty_ReturnTrue)
{
	MapTile tile;
	tile.setTextureName("Tex1");
	tile.setTextureIndex(0);
	tile.setObjectTextureName("Tex1");
	tile.setObjectTextureIndex(0);
	ASSERT_TRUE(tile.isAssigned());
}

TEST(MapTile_setTextureName, WithEmptyName_ReturnSuccess)
{
	MapTile tile;
	tile.setTextureName("ABC");
	ASSERT_EQ("ABC", tile.getTextureName());
	tile.setTextureName("");
	ASSERT_EQ("", tile.getTextureName());
}

TEST(MapTile_setTextureIndex, With1_ReturnSuccess)
{
	MapTile tile;
	tile.setTextureIndex(1);
	ASSERT_EQ(1, tile.getTextureIndex());
}

TEST(MapTile_setTextureIndex, WithMinus1_ReturnSuccess)
{
	MapTile tile;
	tile.setTextureIndex(1);
	ASSERT_EQ(1, tile.getTextureIndex());
	tile.setTextureIndex(-1);
	ASSERT_EQ(-1, tile.getTextureIndex());
}

TEST(MapTile_setObjectTextureName, WithABC_ReturnSuccess)
{
	MapTile tile;
	tile.setObjectTextureName("ABC");
	ASSERT_EQ("ABC", tile.getObjectTextureName());
}

TEST(MapTile_setObjectTextureName, WithEmptyName_ReturnSuccess)
{
	MapTile tile;
	tile.setObjectTextureName("ABC");
	ASSERT_EQ("ABC", tile.getObjectTextureName());
	tile.setObjectTextureName("");
	ASSERT_EQ("", tile.getObjectTextureName());
}

TEST(MapTile_setObjectTextureIndex, With1_ReturnSuccess)
{
	MapTile tile;
	tile.setObjectTextureIndex(1);
	ASSERT_EQ(1, tile.getObjectTextureIndex());
}

TEST(MapTile_setObjectTextureIndex, WithMinus1_ReturnSuccess)
{
	MapTile tile;
	tile.setObjectTextureIndex(1);
	ASSERT_EQ(1, tile.getObjectTextureIndex());
	tile.setObjectTextureIndex(-1);
	ASSERT_EQ(-1, tile.getObjectTextureIndex());
}

TEST(MapTile_getTriggers, WithDefaultTile_ReturnNoTriggers)
{
	MapTile tile;
	ASSERT_EQ(0, tile.getTriggers().size());
}

TEST_F(MapTileWith2Triggers, addTrigger_WithOneAddedTrigger_ReturnSuccess)
{
	ASSERT_EQ(2, tile.getTriggers().size());
	tile.addTrigger(MapTileTrigger(MapTileTriggerEvent::MoveLeftPressed,
								   MapTileTriggerCondition::None,
								   MapTileTriggerAction::OpenChest,
								   map<string, string>()));
	ASSERT_EQ(3, tile.getTriggers().size());
}

TEST_F(MapTileWith2Triggers, findTrigger_WithExistantEvent_ReturnEmpty)
{
	auto actual { tile.findTrigger(MapTileTriggerEvent::MoveDownPressed) };
	ASSERT_TRUE(actual.has_value());
	ASSERT_EQ(MapTileTriggerAction::ChangeMap, actual->getAction());
}

TEST_F(MapTileWith2Triggers, findTrigger_WithNonExistantEvent_ReturnEmpty)
{
	auto actual { tile.findTrigger(MapTileTriggerEvent::MoveRightPressed) };
	ASSERT_FALSE(actual.has_value());
}

TEST_F(MapTileWith2Triggers, updateTrigger_WithDifferentEvent_ReturnSuccess)
{
	auto updatedTrigger { MapTileTrigger(MapTileTriggerEvent::MoveLeftPressed,
										 MapTileTriggerCondition::None,
										 MapTileTriggerAction::ChangeMap,
										 map<string, string>()) };
	ASSERT_TRUE(tile.updateTrigger(secondTrigger, updatedTrigger));
	ASSERT_EQ(MapTileTriggerEvent::MoveLeftPressed, tile.getTriggers()[1].getEvent());
	ASSERT_EQ(MapTileTriggerCondition::None, tile.getTriggers()[1].getCondition());
	ASSERT_EQ(MapTileTriggerAction::ChangeMap, tile.getTriggers()[1].getAction());
	ASSERT_EQ(0, tile.getTriggers()[1].getActionProperties().size());
}

TEST_F(MapTileWith2Triggers, updateTrigger_WithDifferentEventAndCondition_ReturnSuccess)
{
	auto updatedTrigger { MapTileTrigger(MapTileTriggerEvent::MoveLeftPressed,
										 MapTileTriggerCondition::MustBeFacing,
										 MapTileTriggerAction::ChangeMap,
										 map<string, string>()) };
	ASSERT_TRUE(tile.updateTrigger(secondTrigger, updatedTrigger));
	ASSERT_EQ(MapTileTriggerEvent::MoveLeftPressed, tile.getTriggers()[1].getEvent());
	ASSERT_EQ(MapTileTriggerCondition::MustBeFacing, tile.getTriggers()[1].getCondition());
	ASSERT_EQ(MapTileTriggerAction::ChangeMap, tile.getTriggers()[1].getAction());
	ASSERT_EQ(0, tile.getTriggers()[1].getActionProperties().size());
}

TEST_F(MapTileWith2Triggers, updateTrigger_WithDifferentActionProperties_ReturnSuccess)
{
	auto updatedTrigger { MapTileTrigger(MapTileTriggerEvent::MoveLeftPressed,
										 MapTileTriggerCondition::MustBeFacing,
										 MapTileTriggerAction::ChangeMap,
										 map<string, string>({ {"Test", "Test2"} }) ) };
	ASSERT_TRUE(tile.updateTrigger(secondTrigger, updatedTrigger));
	ASSERT_EQ(MapTileTriggerEvent::MoveLeftPressed, tile.getTriggers()[1].getEvent());
	ASSERT_EQ(MapTileTriggerCondition::MustBeFacing, tile.getTriggers()[1].getCondition());
	ASSERT_EQ(MapTileTriggerAction::ChangeMap, tile.getTriggers()[1].getAction());
	ASSERT_EQ(1, tile.getTriggers()[1].getActionProperties().size());
	ASSERT_EQ("Test2", tile.getTriggers()[1].getActionProperties().at("Test"));
}

TEST_F(MapTileWith2Triggers, deleteTrigger_WithFirstTrigger_ReturnTrue)
{
	ASSERT_EQ(2, tile.getTriggers().size());
	ASSERT_TRUE(tile.deleteTrigger(firstTrigger));
	ASSERT_EQ(1, tile.getTriggers().size());
	ASSERT_EQ(secondTrigger.getEvent(), tile.getTriggers()[0].getEvent());
	ASSERT_EQ(secondTrigger.getCondition(), tile.getTriggers()[0].getCondition());
	ASSERT_EQ(secondTrigger.getAction(), tile.getTriggers()[0].getAction());
	ASSERT_EQ(0, tile.getTriggers()[0].getActionProperties().size());

}

TEST_F(MapTileWith2Triggers, deleteTrigger_WithSecondTrigger_ReturnTrue)
{
	ASSERT_EQ(2, tile.getTriggers().size());
	ASSERT_TRUE(tile.deleteTrigger(secondTrigger));
	ASSERT_EQ(1, tile.getTriggers().size());
	ASSERT_EQ(firstTrigger.getEvent(), tile.getTriggers()[0].getEvent());
	ASSERT_EQ(firstTrigger.getCondition(), tile.getTriggers()[0].getCondition());
	ASSERT_EQ(firstTrigger.getAction(), tile.getTriggers()[0].getAction());
	ASSERT_EQ(0, tile.getTriggers()[0].getActionProperties().size());
}

TEST_F(MapTileWith2Triggers, deleteTrigger_WithInexistantTrigger_ReturnFalse)
{
	auto inexistantTrigger { MapTileTrigger(MapTileTriggerEvent::MoveLeftPressed,
								   			MapTileTriggerCondition::None,
								     		MapTileTriggerAction::ChangeMap,
								     		map<string, string>()) };

	ASSERT_EQ(2, tile.getTriggers().size());
	ASSERT_FALSE(tile.deleteTrigger(inexistantTrigger));
	ASSERT_EQ(2, tile.getTriggers().size());
}
