#pragma once

#include <string>
#include <vector>
#include "point.hpp"

namespace thewarrior::models {

enum class NPCFacing { Left, Up, Right, Down };

struct NPCCreationInfo {
    std::string id;
    std::string name;
    Point<size_t> spawnPosition = Point<size_t>(0, 0);
    std::vector<Point<size_t>> wanderZone {};
    std::vector<std::string> dialogueLines {};
    NPCFacing defaultFacing = NPCFacing::Down;
    NPCFacing currentFacing = NPCFacing::Down;
};

class NPC {
 public:
     explicit NPC(const NPCCreationInfo &info);
     const std::string &getId() const;
     const std::string &getName() const;
     const Point<size_t> &getSpawnPosition() const;
     const std::vector<Point<size_t>> &getWanderZone() const;
     const::std::vector<std::string> &getDialogueLines() const;
     NPCFacing getDefaultFacing() const;
     NPCFacing getCurrentFacing() const;
     void setId(const std::string &id);
     void setName(const std::string &name);
     void setSpawnPosition(const Point<size_t> &position);
     void setWanderZone(const std::vector<Point<size_t>> &zone);
     void clearWanderZone();
     void addToWanderZone(const std::vector<Point<size_t>> &zone);
     void removeFromWanderZone(const std::vector<Point<size_t>> &zone);
     void setDialogueLines(const::std::vector<std::string> &lines);
     void setDefaultFacing(NPCFacing value);
     void setCurrentFacing(NPCFacing value);

 private:
     std::string m_id;
     std::string m_name;
     Point<size_t> m_spawnPosition;
     std::vector<Point<size_t>> m_wanderZone;
     std::vector<std::string> m_dialogueLines;
     NPCFacing m_defaultFacing;
     NPCFacing m_currentFacing;
     void validateId(const std::string &id);
     void validateName(const std::string &name);
};

}  // namespace thewarrior::models
