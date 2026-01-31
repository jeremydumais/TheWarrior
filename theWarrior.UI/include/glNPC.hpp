#pragma  once

#include "direction.hpp"
#include "npc.hpp"

namespace thewarrior::ui {

class GLNPC : public thewarrior::models::NPC {
 public:
     explicit GLNPC(const thewarrior::models::NPC &npc);
     ~GLNPC() override;
 private:
     thewarrior::models::Direction m_direction;
};

}  // namespace thewarrior::ui
