#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
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
m_defaultFacing(info.defaultFacing),
m_currentFacing(info.currentFacing),
m_defaultBehavior(info.defaultBehavior),
m_currentBehavior(info.currentBehavior),
m_visibilityRule(info.visibilityRule) {
    validateId(info.id);
    validateName(info.name);
    if (!info.conversationScenarios.empty()) {
        setConversationScenarios(info.conversationScenarios);
    } else {
        migrateDialogueLines(info.dialogueLines);
    }
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

const std::vector<ConversationScenario> &NPC::getConversationScenarios() const {
    return m_conversationScenarios;
}

NPCFacing NPC::getDefaultFacing() const {
    return m_defaultFacing;
}

NPCFacing NPC::getCurrentFacing() const {
    return m_currentFacing;
}

NPCBehavior NPC::getDefaultBehavior() const {
    return m_defaultBehavior;
}

NPCBehavior NPC::getCurrentBehavior() const {
    return m_currentBehavior;
}
    
const std::optional<NPCVisibilityRule> &NPC::getVisibilityRule() const {
    return m_visibilityRule;
}

bool NPC::isVisible(const std::set<StoryId> &completedStoryIds) const {
    if (!m_visibilityRule.has_value()) {
        return true;
    }

    const auto &rule = *m_visibilityRule;

    switch (rule.condition) {
        case NPCVisibilityCondition::AnyStoryCompleted:
            return std::ranges::any_of(
                rule.storyIds,
                [&completedStoryIds](const StoryId &storyId) {
                    return completedStoryIds.contains(storyId);
                });

        case NPCVisibilityCondition::NoStoryCompleted:
            return std::ranges::none_of(
                rule.storyIds,
                [&completedStoryIds](const StoryId &storyId) {
                    return completedStoryIds.contains(storyId);
                });
    }

    return true;
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

void NPC::setBaseTextureIndex(int index) {
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
    std::ranges::for_each(zone, [this](const auto &zonePoint) {
        if (std::find(m_wanderZone.begin(), m_wanderZone.end(), zonePoint) == m_wanderZone.end()) {
            this->m_wanderZone.push_back(zonePoint);
        }
    });
}

void NPC::removeFromWanderZone(const std::vector<Point<size_t>> &zone) {
    std::ranges::for_each(zone, [this](const auto &zonePoint) {
        if (const auto &pointFound = std::find(m_wanderZone.begin(), m_wanderZone.end(), zonePoint); pointFound != m_wanderZone.end()) {
            this->m_wanderZone.erase(pointFound);
        }
    });
}

void NPC::setDialogueLines(const::std::vector<std::string> &lines) {
    if (m_conversationScenarios.empty()) {
        migrateDialogueLines(lines);
        return;
    }
    m_conversationScenarios.front().setDialogueLines(lines);
    refreshDialogueLinesCompatibilityView();
}

void NPC::setConversationScenarios(const std::vector<ConversationScenario> &scenarios) {
    m_conversationScenarios = scenarios;
    refreshDialogueLinesCompatibilityView();
}

void NPC::addConversationScenario(const ConversationScenario &scenario) {
    m_conversationScenarios.push_back(scenario);
    refreshDialogueLinesCompatibilityView();
}

bool NPC::removeConversationScenario(const ConversationScenarioId &scenarioId) {
    const auto iter = std::find_if(
        m_conversationScenarios.begin(),
        m_conversationScenarios.end(),
        [&scenarioId](const ConversationScenario &scenario) {
            return scenario.getId() == scenarioId;
        });
    if (iter == m_conversationScenarios.end()) {
        return false;
    }
    m_conversationScenarios.erase(iter);
    refreshDialogueLinesCompatibilityView();
    return true;
}

void NPC::setDefaultFacing(NPCFacing value) {
    m_defaultFacing = value;
}

void NPC::setCurrentFacing(NPCFacing value) {
    m_currentFacing = value;
}

void NPC::setDefaultBehavior(NPCBehavior value) {
    m_defaultBehavior = value;
}

void NPC::setCurrentBehavior(NPCBehavior value) {
    m_currentBehavior = value;
}

void NPC::setVisibilityRule(const NPCVisibilityRule &rule) {
    m_visibilityRule = rule;
}
    
void NPC::clearVisibilityRule() {
    m_visibilityRule = std::nullopt;
}

void NPC::applyCoordinateOffset(int offsetX, int offsetY) {
    m_spawnPosition.setX(static_cast<size_t>(static_cast<std::int64_t>(m_spawnPosition.x()) + offsetX));
    m_spawnPosition.setY(static_cast<size_t>(static_cast<std::int64_t>(m_spawnPosition.y()) + offsetY));

    for (auto &point : m_wanderZone) {
        point.setX(static_cast<size_t>(static_cast<std::int64_t>(point.x()) + offsetX));
        point.setY(static_cast<size_t>(static_cast<std::int64_t>(point.y()) + offsetY));
    }
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

void NPC::migrateDialogueLines(const std::vector<std::string> &lines) {
    m_dialogueLines = lines;
    m_conversationScenarios.clear();
    if (!lines.empty()) {
        m_conversationScenarios.push_back(
            ConversationScenario::fromLegacyDialogueLines(lines));
    }
}

void NPC::refreshDialogueLinesCompatibilityView() {
    m_dialogueLines.clear();
    if (!m_conversationScenarios.empty()) {
        m_dialogueLines = m_conversationScenarios.front().getDialogueLines();
    }
}

}  // namespace thewarrior::models
