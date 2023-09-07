#include "monsterZone.hpp"
#include <fmt/format.h>
#include <algorithm>
#include <optional>
#include <stdexcept>
#include "monster.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace boost::algorithm;

namespace thewarrior::models {

CompareMonsterEncounter::CompareMonsterEncounter(const std::string &id)
    : m_id(id) {}

bool CompareMonsterEncounter::operator() (const MonsterZoneMonsterEncounter &monsterEncounter) {
    return to_lower_copy(trim_copy(monsterEncounter.getMonsterId())) == to_lower_copy(m_id);
}

MonsterZone::MonsterZone(const std::string &name, const RGBItemColor &color)
    : MonsterZone(name, color, 1, 10) {
}

MonsterZone::MonsterZone(const std::string &name,
                         const RGBItemColor &color,
                         const unsigned int ratioEncounter,
                         const unsigned int rationEncounterOn,
                         const std::vector<MonsterZoneMonsterEncounter> &monsterEncounters)
    : m_name(name),
      m_color(color),
      m_ratioEncounter(ratioEncounter),
      m_ratioEncounterOn(rationEncounterOn),
      m_monsterEncounters(monsterEncounters) {
    validateName(m_name);
    validateRatioEncounter(m_ratioEncounter, m_ratioEncounterOn);
}

// Needed for deserialization
MonsterZone::MonsterZone()
    :   m_name("Default"),
        m_color("White", "#FFFFFF"),
        m_ratioEncounter(1),
        m_ratioEncounterOn(10),
        m_monsterEncounters({})
{}

const std::string &MonsterZone::getLastError() const {
    return m_lastError;
}

const std::string &MonsterZone::getName() const {
    return m_name;
}

const RGBItemColor &MonsterZone::getColor() const {
    return m_color;
}

unsigned int MonsterZone::getRatioEncounter() const {
    return m_ratioEncounter;
}

unsigned int MonsterZone::getRatioEncounterOn() const {
    return m_ratioEncounterOn;
}

std::optional<MonsterZoneMonsterEncounter> MonsterZone::getMonsterEncounter(const std::string &monsterId) const {
    const auto item = findMonsterEncounter(monsterId);
    if (item == m_monsterEncounters.end()) {
        return std::nullopt;
    }
    return *item;
}

const std::vector<MonsterZoneMonsterEncounter> &MonsterZone::getMonsterEncounters() const {
    return m_monsterEncounters;
}

void MonsterZone::setName(const std::string &name) {
    validateName(name);
    m_name = name;
}

void MonsterZone::setColor(const RGBItemColor &color) {
    m_color = color;
}

void MonsterZone::setRatioEncounter(const unsigned int value) {
    validateRatioEncounter(value, m_ratioEncounterOn);
    m_ratioEncounter = value;
}

void MonsterZone::setRatioEncounterOn(const unsigned int value) {
    validateRatioEncounter(m_ratioEncounter, value);
    m_ratioEncounterOn = value;
}

bool MonsterZone::addMonsterEncounter(const MonsterZoneMonsterEncounter &monsterEncounter) {
    const auto &id = monsterEncounter.getMonsterId();
    if (findMonsterEncounter(id) != m_monsterEncounters.end()) {
        m_lastError = fmt::format("id {0} already exists.", id);
        return false;
    }
    m_monsterEncounters.push_back(monsterEncounter);
    return true;
}

bool MonsterZone::replaceMonsterEncounter(const MonsterZoneMonsterEncounter &oldMonsterEncounter,
                                          const MonsterZoneMonsterEncounter &newMonsterEncounter) {
    const auto &oldId = oldMonsterEncounter.getMonsterId();
    const auto &newId = newMonsterEncounter.getMonsterId();
    auto oldItem = findMonsterEncounter(oldId);
    if (oldItem == m_monsterEncounters.end()) {
        m_lastError = fmt::format("oldId {0} doesn't exist in the list.", oldId);
        return false;
    }
    if (findMonsterEncounter(newId) != m_monsterEncounters.end()) {
        m_lastError = fmt::format("newId {0} is already in the list.", newId);
        return false;
    }
    *oldItem = newMonsterEncounter;
    return true;
}

bool MonsterZone::removeMonsterEncounter(const std::string &monsterId) {
    const auto iter = findMonsterEncounter(monsterId);
    if (iter == m_monsterEncounters.end()) {
        m_lastError = fmt::format("monsterId {0} doesn't exist in the list.", monsterId);
        return false;
    }
    m_monsterEncounters.erase(iter);
    return true;
}

void MonsterZone::validateName(const std::string &name) const {
    if (trim_copy(name).empty()) {
        throw std::invalid_argument("name cannot be null or empty.");
    }
}

void MonsterZone::validateRatioEncounter(const unsigned int ratioEncounter,
                                         const unsigned int ratioEncounterOn) {
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

std::vector<MonsterZoneMonsterEncounter>::const_iterator MonsterZone::findMonsterEncounter(const std::string &id) const {
    CompareMonsterEncounter comparer(id);
    return std::find_if(m_monsterEncounters.cbegin(),
                        m_monsterEncounters.cend(),
                        comparer);
}

std::vector<MonsterZoneMonsterEncounter>::iterator MonsterZone::findMonsterEncounter(const std::string &id) {
    CompareMonsterEncounter comparer(id);
    return std::find_if(m_monsterEncounters.begin(),
                        m_monsterEncounters.end(),
                        comparer);
}

}  // namespace thewarrior::models
