#include <algorithm>
#include <string>
#include <vector>
#include "npc.hpp"
#include <boost/algorithm/string/trim.hpp>

namespace thewarrior::models {

NPC::NPC(const NPCCreationInfo &info)
: m_id(info.id),
m_name(info.name),
m_textureName(info.textureName),
m_baseTextureIndex(info.baseTextureIndex),
m_spawnPosition(info.spawnPosition),
m_wanderZone(info.wanderZone),
m_dialogueLines(info.dialogueLines),
m_defaultFacing(info.defaultFacing),
m_currentFacing(info.currentFacing) {
    validateId(info.id);
    validateName(info.name);
}

const std::string &NPC::getId() const {
    return m_id;
}

const std::string &NPC::getName() const {
    return m_name;
}

const std::string &NPC::getTextureName() const {
    return m_textureName;
}

int NPC::getBaseTextureIndex() const {
    return m_baseTextureIndex;
}

const Point<size_t> &NPC::getSpawnPosition() const {
    return m_spawnPosition;
}

const std::vector<Point<size_t>> &NPC::getWanderZone() const {
    return m_wanderZone;
}

const::std::vector<std::string> &NPC::getDialogueLines() const {
    return m_dialogueLines;
}

NPCFacing NPC::getDefaultFacing() const {
    return m_defaultFacing;
}

NPCFacing NPC::getCurrentFacing() const {
    return m_currentFacing;
}

int NPC::getCurrentFacingTextureIndex() const {
    switch (m_currentFacing) {
        case NPCFacing::Left:
            return m_baseTextureIndex + 25;
        case NPCFacing::Right:
            return m_baseTextureIndex + 13;
        case NPCFacing::Up:
            return m_baseTextureIndex + 1;
        default:
            return m_baseTextureIndex + 37;
    }
}

void NPC::setId(const std::string &id) {
    validateId(id);
    m_id = id;
}

void NPC::setName(const std::string &name) {
    validateName(name);
    m_name = name;
}


void NPC::setTextureName(const std::string &textureName) {
    m_textureName = textureName;
}

void NPC::setBaseTextureIndex(const int index) {
    m_baseTextureIndex = index;
}

void NPC::setSpawnPosition(const Point<size_t> &position) {
    m_spawnPosition = position;
}

void NPC::setWanderZone(const std::vector<Point<size_t>> &wanderZone) {
    m_wanderZone = wanderZone;
}

void NPC::clearWanderZone() {
    m_wanderZone.clear();
}

void NPC::addToWanderZone(const std::vector<Point<size_t>> &zone) {
    std::for_each(zone.begin(), zone.end(), [this](const auto &zonePoint) {
        if (std::find(m_wanderZone.begin(), m_wanderZone.end(), zonePoint) == m_wanderZone.end()) {
            this->m_wanderZone.push_back(zonePoint);
        }
    });
}

void NPC::removeFromWanderZone(const std::vector<Point<size_t>> &zone) {
    std::for_each(zone.begin(), zone.end(), [this](const auto &zonePoint) {
        if (const auto &pointFound = std::find(m_wanderZone.begin(), m_wanderZone.end(), zonePoint); pointFound != m_wanderZone.end()) {
            this->m_wanderZone.erase(pointFound);
        }
    });
}

void NPC::setDialogueLines(const::std::vector<std::string> &lines) {
    m_dialogueLines = lines;
}

void NPC::setDefaultFacing(NPCFacing value) {
    m_defaultFacing = value;
}

void NPC::setCurrentFacing(NPCFacing value) {
    m_currentFacing = value;
}

void NPC::validateId(const std::string &id) {
    std::string sanitizedId { boost::trim_copy(id) };
    if (sanitizedId.empty()) {
        throw std::invalid_argument("id cannot be empty.");
    }
    if (sanitizedId.length() != 6) {
        throw std::invalid_argument("id must be 6 characters long.");
    }
}

void NPC::validateName(const std::string &name) {
    std::string sanitizedName { boost::trim_copy(name) };
    if (sanitizedName.empty()) {
        throw std::invalid_argument("name cannot be empty.");
    }
}

}  // namespace thewarrior::models


