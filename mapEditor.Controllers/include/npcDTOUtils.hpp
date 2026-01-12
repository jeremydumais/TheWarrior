#pragma once

#include <optional>
#include <string>
#include "npcDTO.hpp"
#include "npc.hpp"

namespace mapeditor::controllers {

class NPCDTOUtils {
 public:
    struct NPCConversionResult {
        std::optional<thewarrior::models::NPC> npc;
        std::string errorMessage = "";

        bool success() const noexcept { return npc.has_value(); }
    };
    static NPCDTO fromNPC(const thewarrior::models::NPC &npc);
    static NPCConversionResult toNPC(const NPCDTO &dto);
};

}  // namespace mapeditor::controllers

