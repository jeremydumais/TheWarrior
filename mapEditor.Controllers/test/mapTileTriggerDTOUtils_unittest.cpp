#include <gtest/gtest.h>
#include "conversationScenario.hpp"
#include "mapTileTrigger.hpp"
#include "mapTileTriggerDTO.hpp"
#include "mapTileTriggerDTOUtils.hpp"

using mapeditor::controllers::MapTileTriggerDTO;
using mapeditor::controllers::MapTileTriggerDTOUtils;
using thewarrior::models::ConversationChoice;
using thewarrior::models::ConversationChoiceOption;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationScenario;
using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerAction;
using thewarrior::models::MapTileTriggerCondition;
using thewarrior::models::MapTileTriggerEvent;

namespace {

ConversationScenario createBedRestScenario() {
    return ConversationScenario(
        "bed-rest",
        "",
        {
            ConversationNode(
                "prompt",
                ConversationChoice {
                    .prompt = "Would you like to rest?",
                    .options = {
                        ConversationChoiceOption {
                            .text = "Yes",
                            .nextNodeId = "rest"
                        },
                        ConversationChoiceOption {
                            .text = "No",
                            .nextNodeId = "stop"
                        }
                    }
                })
        });
}

}  // namespace

TEST(MapTileTriggerDTOUtils_fromMapTileTrigger,
     WithConversationScenarioPreserveTypedScenario) {
    MapTileTrigger trigger;
    trigger.setEvent(MapTileTriggerEvent::ActionButtonPressed);
    trigger.setCondition(MapTileTriggerCondition::None);
    trigger.setConversationScenario(createBedRestScenario());

    const auto dto = MapTileTriggerDTOUtils::fromMapTileTrigger(trigger);

    EXPECT_EQ("ActionButtonPressed", dto.event);
    EXPECT_EQ("ConversationScenario", dto.action);
    EXPECT_TRUE(dto.actionProperties.empty());
    ASSERT_TRUE(dto.conversationScenario.has_value());
    EXPECT_EQ(createBedRestScenario(), *dto.conversationScenario);
}

TEST(MapTileTriggerDTOUtils_toMapTileTrigger,
     WithConversationScenarioPreserveTypedScenario) {
    const MapTileTriggerDTO dto {
        .event = "ActionButtonPressed",
        .condition = "None",
        .action = "ConversationScenario",
        .actionProperties = {},
        .conversationScenario = createBedRestScenario()
    };

    const auto trigger = MapTileTriggerDTOUtils::toMapTileTrigger(dto);

    EXPECT_EQ(
        MapTileTriggerAction::ConversationScenario,
        trigger.getAction());
    ASSERT_NE(nullptr, trigger.getConversationScenario());
    EXPECT_EQ(createBedRestScenario(), *trigger.getConversationScenario());
}

TEST(MapTileTriggerDTOUtils_toMapTileTrigger,
     WithMissingConversationScenarioThrowInvalidArgument) {
    const MapTileTriggerDTO dto {
        .event = "ActionButtonPressed",
        .condition = "None",
        .action = "ConversationScenario"
    };

    EXPECT_THROW(
        MapTileTriggerDTOUtils::toMapTileTrigger(dto),
        std::invalid_argument);
}

TEST(MapTileTriggerDTOUtils_RoundTrip, WithLegacyActionPreserveProperties) {
    const MapTileTrigger trigger(
        MapTileTriggerEvent::ActionButtonPressed,
        MapTileTriggerCondition::None,
        MapTileTriggerAction::OpenChest,
        {{"itemId", "item001"}});

    const auto actual = MapTileTriggerDTOUtils::toMapTileTrigger(
        MapTileTriggerDTOUtils::fromMapTileTrigger(trigger));

    EXPECT_EQ(trigger, actual);
}
