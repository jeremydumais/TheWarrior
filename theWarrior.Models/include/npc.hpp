#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include "conversationScenario.hpp"
#include "point.hpp"

namespace thewarrior::models {

enum class NPCBehavior { Stationary, Wander };
enum class NPCFacing { Left, Up, Right, Down };

struct NPCCreationInfo {
    std::string id;
    std::string name;
    std::string textureName;
    int baseTextureIndex = -1;
    Point<size_t> spawnPosition = Point<size_t>(0, 0);
    std::vector<Point<size_t>> wanderZone;
    // Legacy construction input. Prefer conversationScenarios for new code.
    std::vector<std::string> dialogueLines;
    std::vector<ConversationScenario> conversationScenarios;
    NPCFacing defaultFacing = NPCFacing::Down;
    NPCFacing currentFacing = NPCFacing::Down;
    NPCBehavior defaultBehavior = NPCBehavior::Stationary;
    NPCBehavior currentBehavior = NPCBehavior::Stationary;
};

class NPC {
 public:
    explicit NPC(const NPCCreationInfo &info);
    virtual ~NPC() = default;
    NPC(const NPC&) = default;
    NPC(NPC&&) = default;
    NPC& operator=(const NPC&) = default;
    NPC& operator=(NPC&&) = default;
    const std::string &getId() const;
    const std::string &getName() const;
    const std::string &getTextureName() const;
    int getBaseTextureIndex() const;
    const Point<size_t> &getSpawnPosition() const;
    const std::vector<Point<size_t>> &getWanderZone() const;
    const::std::vector<std::string> &getDialogueLines() const;
    const std::vector<ConversationScenario> &getConversationScenarios() const;
    NPCFacing getDefaultFacing() const;
    NPCFacing getCurrentFacing() const;
    NPCBehavior getDefaultBehavior() const;
    NPCBehavior getCurrentBehavior() const;
    int getCurrentFacingTextureIndex() const;
    void setId(const std::string &id);
    void setName(const std::string &name);
    void setTextureName(const std::string &textureName);
    void setBaseTextureIndex(int index);
    void setSpawnPosition(const Point<size_t> &position);
    void setWanderZone(const std::vector<Point<size_t>> &zone);
    void clearWanderZone();
    void addToWanderZone(const std::vector<Point<size_t>> &zone);
    void removeFromWanderZone(const std::vector<Point<size_t>> &zone);
    void setDialogueLines(const::std::vector<std::string> &lines);
    void setConversationScenarios(const std::vector<ConversationScenario> &scenarios);
    void addConversationScenario(const ConversationScenario &scenario);
    bool removeConversationScenario(const ConversationScenarioId &scenarioId);
    void setDefaultFacing(NPCFacing value);
    void setCurrentFacing(NPCFacing value);
    void setDefaultBehavior(NPCBehavior value);
    void setCurrentBehavior(NPCBehavior value);
    void applyCoordinateOffset(int offsetX, int offsetY);

 private:
    friend class boost::serialization::access;
    NPC() = default;   // Needed for deserialization
    std::string m_id;
    std::string m_name;
    std::string m_textureName;
    int m_baseTextureIndex = -1;
    Point<size_t> m_spawnPosition = Point<size_t>(0, 0);
    std::vector<Point<size_t>> m_wanderZone;
    // Compatibility view for callers not yet migrated to conversation scenarios.
    // It is never serialized by version 3.
    std::vector<std::string> m_dialogueLines;
    std::vector<ConversationScenario> m_conversationScenarios;
    NPCFacing m_defaultFacing = NPCFacing::Down;
    NPCFacing m_currentFacing = NPCFacing::Down;
    NPCBehavior m_defaultBehavior = NPCBehavior::Stationary;
    NPCBehavior m_currentBehavior = NPCBehavior::Stationary;

    void validateId(const std::string &id);
    void validateName(const std::string &name);
    void migrateDialogueLines(const std::vector<std::string> &lines);
    void refreshDialogueLinesCompatibilityView();
    // Serialization methods
    template<class Archive>
    void save(Archive& ar, const unsigned int /* version */) const {
        ar & m_id;
        ar & m_name;
        ar & m_textureName;
        ar & m_baseTextureIndex;
        ar & m_spawnPosition;
        ar & m_wanderZone;
        ar & m_conversationScenarios;
        ar & m_defaultFacing;
        ar & m_currentFacing;
        ar & m_defaultBehavior;
        ar & m_currentBehavior;
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version) {
        ar & m_id;
        ar & m_name;
        ar & m_textureName;
        ar & m_baseTextureIndex;
        ar & m_spawnPosition;
        ar & m_wanderZone;

        if (version <= 2) {
            std::vector<std::string> legacyDialogueLines;

            // Keep the exact field order used by NPC archive versions 0-2.
            ar & legacyDialogueLines;
            ar & m_defaultFacing;
            ar & m_currentFacing;
            if (version >= 2) {
                ar & m_defaultBehavior;
                ar & m_currentBehavior;
            } else {
                m_defaultBehavior = NPCBehavior::Stationary;
                m_currentBehavior = NPCBehavior::Stationary;
            }
            migrateDialogueLines(legacyDialogueLines);
        } else {
            ar & m_conversationScenarios;
            ar & m_defaultFacing;
            ar & m_currentFacing;
            ar & m_defaultBehavior;
            ar & m_currentBehavior;
            refreshDialogueLinesCompatibilityView();
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::NPC, 3)
