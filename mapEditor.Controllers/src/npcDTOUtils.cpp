#include <optional>
#include <stdexcept>
#include "npcDTOUtils.hpp"
#include "npc.hpp"
#include "npcDTO.hpp"

using mapeditor::controllers::NPCDTO;
using thewarrior::models::NPC;
using thewarrior::models::NPCCreationInfo;

namespace mapeditor::controllers
{

    NPCDTO NPCDTOUtils::fromNPC(const NPC &npc) {
        return NPCDTO{
            .id = npc.getId(),
            .name = npc.getName(),
            .textureName = npc.getTextureName(),
            .baseTextureIndex = npc.getBaseTextureIndex(),
            .spawnPosition = npc.getSpawnPosition(),
            .wanderZone = npc.getWanderZone(),
            .conversationScenarios = npc.getConversationScenarios(),
            .defaultFacing = npc.getDefaultFacing(),
            .currentFacing = npc.getCurrentFacing(),
            .defaultBehavior = npc.getDefaultBehavior(),
            .currentBehavior = npc.getCurrentBehavior(),
            .visibilityRule = npc.getVisibilityRule(),
            .spriteLayout = npc.getSpriteLayout()
        };
    }

    NPCDTOUtils::NPCConversionResult NPCDTOUtils::toNPC(const NPCDTO &dto) {
        NPCCreationInfo creationInfo{
            .id = dto.id,
            .name = dto.name,
            .textureName = dto.textureName,
            .baseTextureIndex = dto.baseTextureIndex,
            .spawnPosition = dto.spawnPosition,
            .wanderZone = dto.wanderZone,
            .dialogueLines = {},
            .conversationScenarios = dto.conversationScenarios,
            .defaultFacing = dto.defaultFacing,
            .currentFacing = dto.currentFacing,
            .defaultBehavior = dto.defaultBehavior,
            .currentBehavior = dto.currentBehavior,
            .visibilityRule = dto.visibilityRule,
            .spriteLayout = dto.spriteLayout,
        };
        try {
            NPC result(creationInfo);
            return {.npc = result, .errorMessage = ""};
        }
        catch (const std::invalid_argument &err) {
            return {.npc = std::nullopt, .errorMessage = err.what()};
        }
    }

} // namespace mapeditor::controllers
