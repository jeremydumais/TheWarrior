#include "monsterZoneMonsterEncounter.hpp"
#include <stdexcept>
#include "monster.hpp"

namespace thewarrior::models {

MonsterZoneMonsterEncounter::MonsterZoneMonsterEncounter()
    : MonsterZoneMonsterEncounter("ZZZ000", MonsterEncounterRatio::Normal)
{}

MonsterZoneMonsterEncounter::MonsterZoneMonsterEncounter(const std::string &monsterId,
        MonsterEncounterRatio encounterRatio)
    : m_monsterId(monsterId),
      m_encounterRatio(encounterRatio) {
    Monster::validateId(monsterId);
}

const std::string &MonsterZoneMonsterEncounter::getMonsterId() const {
    return m_monsterId;
}

MonsterEncounterRatio MonsterZoneMonsterEncounter::getEncounterRatio() const {
    return m_encounterRatio;
}

void MonsterZoneMonsterEncounter::setMonsterId(const std::string &id) {
    Monster::validateId(id);
    m_monsterId = id;
}

void MonsterZoneMonsterEncounter::setEncounterRatio(MonsterEncounterRatio ratio) {
    m_encounterRatio = ratio;
}

}  // namespace thewarrior::models
