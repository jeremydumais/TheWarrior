#pragma once

#include "monster.hpp"
#include "rgbItemColor.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
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
                const std::vector<Monster> &monsters = {});
    const std::string &getName() const;
    const RGBItemColor &getColor() const;
    unsigned int getRatioEncounter() const;
    unsigned int getRatioEncounterOn() const;
    const std::vector<Monster> &getMonsters() const;
    void setName(const std::string &name);
    void setColor(const RGBItemColor &color);
    void setRatioEncounter(const unsigned int value);
    void setRatioEncounterOn(const unsigned int value);
private:
    std::string m_name;
    RGBItemColor m_color;
    unsigned int m_ratioEncounter = 1;
    unsigned int m_ratioEncounterOn = 10;
    std::vector<Monster> m_monsters = {};
    void validateName(const std::string &name) const;
    void validateRatioEncounter(const unsigned int ratioEncounter,
                                const unsigned int rationEncounterOn);
};

} // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::MonsterZone, 0)
