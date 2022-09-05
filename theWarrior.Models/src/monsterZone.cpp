#include "monsterZone.hpp"
#include "monster.hpp"
#include "boost/algorithm/string.hpp"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <stdexcept>

using namespace boost::algorithm;

namespace thewarrior::models {

MonsterZone::MonsterZone(const std::string &name, const RGBItemColor &color)
    : MonsterZone(name, color, 1, 10)
{
}

MonsterZone::MonsterZone(const std::string &name,
                         const RGBItemColor &color,
                         const unsigned int ratioEncounter,
                         const unsigned int rationEncounterOn,
                         const std::vector<std::string> &monsterIds)
    : m_name(name),
      m_color(color),
      m_ratioEncounter(ratioEncounter),
      m_ratioEncounterOn(rationEncounterOn),
      m_monsterIds(monsterIds)
{
    validateName(m_name);
    validateRatioEncounter(m_ratioEncounter, m_ratioEncounterOn);
}

//Needed for deserialization
MonsterZone::MonsterZone()
    :   m_name("Default"),
        m_color("White", "#FFFFFF"),
        m_ratioEncounter(1),
        m_ratioEncounterOn(10),
        m_monsterIds({})
{}

const std::string &MonsterZone::getName() const
{
    return m_name;
}

const RGBItemColor &MonsterZone::getColor() const
{
    return m_color;
}

unsigned int MonsterZone::getRatioEncounter() const
{
    return m_ratioEncounter;
}

unsigned int MonsterZone::getRatioEncounterOn() const
{
    return m_ratioEncounterOn;
}

const std::vector<std::string> &MonsterZone::getMonsterIds() const
{
    return m_monsterIds;
}

void MonsterZone::setName(const std::string &name)
{
    validateName(name);
    m_name = name;
}

void MonsterZone::setColor(const RGBItemColor &color)
{
    m_color = color;
}

void MonsterZone::setRatioEncounter(const unsigned int value)
{
    validateRatioEncounter(value, m_ratioEncounterOn);
    m_ratioEncounter = value;
}

void MonsterZone::setRatioEncounterOn(const unsigned int value)
{
    validateRatioEncounter(m_ratioEncounter, value);
    m_ratioEncounterOn = value;
}

void MonsterZone::addMonsterId(const std::string &id)
{
    Monster::validateId(id);
    //Check if already exists
    if (std::find_if(m_monsterIds.begin(),
                     m_monsterIds.end(),
                     [&id] (const std::string &value) {
                        return to_upper_copy(id) == to_upper_copy(value);
                     })  != m_monsterIds.end()) {
        throw std::invalid_argument(fmt::format("id {0} already exists.", id));
    }
    m_monsterIds.push_back(id);
}

void MonsterZone::replaceMonsterId(const std::string &oldId, const std::string &newId)
{
    Monster::validateId(oldId, "oldId");
    Monster::validateId(newId, "newId");
    //TODO Next continue this part
}

void MonsterZone::validateName(const std::string &name) const
{
    if (trim_copy(name).empty()) {
        throw std::invalid_argument("name cannot be null or empty.") ;
    }
}

void MonsterZone::validateRatioEncounter(const unsigned int ratioEncounter,
                                         const unsigned int ratioEncounterOn)
{
    if (ratioEncounter == 0) {
        throw std::invalid_argument("ratio encounter must be greater than zero.");
    }
    if (ratioEncounterOn == 0) {
        throw std::invalid_argument("ratio encounter on must be greater than zero.");
    }
    if (ratioEncounter > ratioEncounterOn) {
        throw std::invalid_argument("ratio encounter cannot be greater than ratio encounter on.");
    }
}

} // namespace thewarrior::models
