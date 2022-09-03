#include "monsterZone.hpp"
#include "boost/algorithm/string.hpp"
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
                         const std::vector<Monster> &monsters)
    : m_name(name),
      m_color(color),
      m_ratioEncounter(ratioEncounter),
      m_ratioEncounterOn(rationEncounterOn),
      m_monsters(monsters)
{
    validateName(m_name);
    validateRatioEncounter(m_ratioEncounter, m_ratioEncounterOn);
}

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

const std::vector<Monster> &MonsterZone::getMonsters() const
{
    return m_monsters;
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
    //TODO NEXT: Code this method
}

void MonsterZone::setRatioEncounterOn(const unsigned int value)
{
    //TODO NEXT: Code this method
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
    if (ratioEncounter > ratioEncounterOn) {
        throw std::invalid_argument("ratio encounter cannot be grater then ratio encounter on.");
    }
}

} // namespace thewarrior::models
