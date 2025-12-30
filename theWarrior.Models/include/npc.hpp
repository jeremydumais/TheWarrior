#pragma once

#include <string>
#include "point.hpp"

namespace thewarrior::models {

struct NPCCreationInfo {
    std::string id;
    std::string name;
    Point<size_t> spawnPosition = Point<size_t>(0, 0);
};

class NPC {
 public:
     explicit NPC(const NPCCreationInfo &info);
     const std::string &getId() const;
     const std::string &getName() const;

     void setId(const std::string &id);
     void setName(const std::string &name);

 private:
     std::string m_id;
     std::string m_name;
     Point<size_t> m_spawnPosition;
     void validateId(const std::string &id);
     void validateName(const std::string &name);
};

}  // namespace thewarrior::models
