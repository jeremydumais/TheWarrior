#include <string>
#include "npc.hpp"
#include <boost/algorithm/string/trim.hpp>

namespace thewarrior::models {

NPC::NPC(const NPCCreationInfo &info)
: m_id(info.id),
m_name(info.name),
m_spawnPosition(info.spawnPosition) {
    validateId(info.id);
    validateName(info.name);
}

const std::string &NPC::getId() const {
    return m_id;
}

const std::string &NPC::getName() const {
    return m_name;
}

void NPC::setId(const std::string &id) {
    validateId(id);
    m_id = id;
}

void NPC::setName(const std::string &name) {
    validateName(name);
    m_name = name;
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


