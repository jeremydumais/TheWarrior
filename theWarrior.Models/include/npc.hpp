#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "point.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

namespace thewarrior::models {

enum class NPCBehavior { Stationary, Wander };
enum class NPCFacing { Left, Up, Right, Down };
#
struct NPCCreationInfo {
    std::string id;
    std::string name;
    std::string textureName;
    int baseTextureIndex = -1;
    Point<size_t> spawnPosition = Point<size_t>(0, 0);
    std::vector<Point<size_t>> wanderZone;
    std::vector<std::string> dialogueLines;
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
    std::vector<std::string> m_dialogueLines;
    NPCFacing m_defaultFacing = NPCFacing::Down;
    NPCFacing m_currentFacing = NPCFacing::Down;
    NPCBehavior m_defaultBehavior = NPCBehavior::Stationary;
    NPCBehavior m_currentBehavior = NPCBehavior::Stationary;

    void validateId(const std::string &id);
    void validateName(const std::string &name);
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & m_id;
        ar & m_name;
        ar & m_textureName;
        ar & m_baseTextureIndex;
        ar & m_spawnPosition;
        ar & m_wanderZone;
        ar & m_dialogueLines;
        ar & m_defaultFacing;
        ar & m_currentFacing;
        if (version >= 2) {
            ar & m_defaultBehavior;
            ar & m_currentBehavior;
        }
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::NPC, 2)
