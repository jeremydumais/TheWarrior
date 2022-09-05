#pragma once

#include "rgbItemColor.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include <string>
#include <vector>

namespace thewarrior::models {

class MonsterZone
{
public:
    MonsterZone(const std::string &name, const RGBItemColor &color);
    MonsterZone(const std::string &name,
                const RGBItemColor &color,
                const unsigned int ratioEncounter,
                const unsigned int rationEncounterOn,
                const std::vector<std::string> &monsterIds = {});
    const std::string &getName() const;
    const RGBItemColor &getColor() const;
    unsigned int getRatioEncounter() const;
    unsigned int getRatioEncounterOn() const;
    const std::vector<std::string> &getMonsterIds() const;
    void setName(const std::string &name);
    void setColor(const RGBItemColor &color);
    void setRatioEncounter(const unsigned int value);
    void setRatioEncounterOn(const unsigned int value);
    void addMonsterId(const std::string &id);
    void replaceMonsterId(const std::string &oldId, const std::string &newId);
private:
    friend class boost::serialization::access;
    MonsterZone();
    std::string m_name;
    RGBItemColor m_color;
    unsigned int m_ratioEncounter = 1;
    unsigned int m_ratioEncounterOn = 10;
    std::vector<std::string> m_monsterIds = {};
    void validateName(const std::string &name) const;
    void validateRatioEncounter(const unsigned int ratioEncounter,
                                const unsigned int rationEncounterOn);
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & m_name;
        ar & m_color;
        ar & m_ratioEncounter;
        ar & m_ratioEncounterOn;
        ar & m_monsterIds;
    }
};

} // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::MonsterZone, 0)
