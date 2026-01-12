#include <optional>
#include <stdexcept>
#include "npcDTOUtils.hpp"
#include "npc.hpp"
#include "npcDTO.hpp"

using thewarrior::models::NPC;
using thewarrior::models::NPCCreationInfo;
using mapeditor::controllers::NPCDTO;

namespace mapeditor::controllers {

NPCDTO NPCDTOUtils::fromNPC(const NPC &npc) {
    return NPCDTO {
        npc.getId(),
        npc.getName(),
        npc.getTextureName(),
        npc.getBaseTextureIndex(),
        npc.getSpawnPosition(),
        npc.getWanderZone(),
        npc.getDialogueLines(),
        npc.getDefaultFacing(),
        npc.getCurrentFacing()
    };
}

NPCDTOUtils::NPCConversionResult NPCDTOUtils::toNPC(const NPCDTO &dto) {
    NPCCreationInfo creationInfo {
        dto.id,
        dto.name,
        dto.textureName,
        dto.baseTextureIndex,
        dto.spawnPosition,
        dto.wanderZone,
        dto.dialogueLines,
        dto.defaultFacing,
        dto.defaultFacing
    };
    try {
        NPC result(creationInfo);
        return {result};
    } catch (const std::invalid_argument &err) {
        return {std::nullopt, err.what()};
    }
}

}  // namespace mapeditor::controllers
