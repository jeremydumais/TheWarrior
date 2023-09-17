#pragma once

#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

namespace thewarrior::models {

enum MonsterEncounterRatio {
    Normal,
    LessThanNormal,
    Rare
};

class MonsterZoneMonsterEncounter {
 public:
    MonsterZoneMonsterEncounter(const std::string &monsterId,
                                MonsterEncounterRatio encounterRatio);
    friend bool operator==(const MonsterZoneMonsterEncounter &lhs, const MonsterZoneMonsterEncounter &rhs);
    friend bool operator!=(const MonsterZoneMonsterEncounter &lhs, const MonsterZoneMonsterEncounter &rhs);
    const std::string &getMonsterId() const;
    MonsterEncounterRatio getEncounterRatio() const;
    void setMonsterId(const std::string &id);
    void setEncounterRatio(MonsterEncounterRatio ratio);

 private:
    friend class boost::serialization::access;
    MonsterZoneMonsterEncounter();
    std::string m_monsterId;
    MonsterEncounterRatio m_encounterRatio;
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & m_monsterId;
        ar & m_encounterRatio;
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::MonsterZoneMonsterEncounter, 0)
