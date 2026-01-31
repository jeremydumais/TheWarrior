#include "glNPC.hpp"
#include "direction.hpp"
#include "npc.hpp"

using thewarrior::models::NPC;

namespace thewarrior::ui {

GLNPC::GLNPC(const NPC &npc)
: NPC(npc),
m_direction(models::Direction::None) {}

GLNPC::~GLNPC() {}

}  // namespace thewarrior::ui
