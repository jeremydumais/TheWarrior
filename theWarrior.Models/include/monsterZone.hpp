#pragma once

#include "monsterZoneMonsterEncounter.hpp"
#include "rgbItemColor.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include <optional>
#include <string>
#include <vector>

namespace thewarrior::models {

struct CompareMonsterEncounter
{
public:
    CompareMonsterEncounter(const std::string &id);
    bool operator() (const MonsterZoneMonsterEncounter &monsterEncounter);
private:
    std::string m_id;
};

class MonsterZone
{
public:
    MonsterZone(const std::string &name, const RGBItemColor &color);
    MonsterZone(const std::string &name,
                const RGBItemColor &color,
                const unsigned int ratioEncounter,
                const unsigned int rationEncounterOn,
                const std::vector<MonsterZoneMonsterEncounter> &monsterEncounters = {});
    const std::string &getName() const;
    const RGBItemColor &getColor() const;
    unsigned int getRatioEncounter() const;
    unsigned int getRatioEncounterOn() const;
    std::optional<MonsterZoneMonsterEncounter> getMonsterEncounter(const std::string &monsterId) const;
    const std::vector<MonsterZoneMonsterEncounter> &getMonsterEncounters() const;
    void setName(const std::string &name);
    void setColor(const RGBItemColor &color);
    void setRatioEncounter(const unsigned int value);
    void setRatioEncounterOn(const unsigned int value);
    void addMonsterEncounter(const MonsterZoneMonsterEncounter &monsterEncounter);
    void replaceMonsterEncounter(const MonsterZoneMonsterEncounter &oldMonsterEncounter,
                                 const MonsterZoneMonsterEncounter &newMonsterEncounter);
    void removeMonsterEncounter(const std::string &monsterId);
private:
    friend class boost::serialization::access;
    MonsterZone();
    std::string m_name;
    RGBItemColor m_color;
    unsigned int m_ratioEncounter = 1;
    unsigned int m_ratioEncounterOn = 10;
    std::vector<MonsterZoneMonsterEncounter> m_monsterEncounters = {};
    void validateName(const std::string &name) const;
    void validateRatioEncounter(const unsigned int ratioEncounter,
                                const unsigned int rationEncounterOn);
    std::vector<MonsterZoneMonsterEncounter>::const_iterator findMonsterEncounter(const std::string &id) const;
    std::vector<MonsterZoneMonsterEncounter>::iterator findMonsterEncounter(const std::string &id);
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & m_name;
        ar & m_color;
        ar & m_ratioEncounter;
        ar & m_ratioEncounterOn;
        ar & m_monsterEncounters;
    }
};

} // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::MonsterZone, 0)
