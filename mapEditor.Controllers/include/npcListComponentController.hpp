#pragma once

#include <optional>
#include <string>
#include <vector>
#include <boost/optional/optional.hpp>
#include "glComponentController.hpp"
#include "npcDTO.hpp"

namespace mapeditor::controllers {

class NPCListComponentController {
 public:
    explicit NPCListComponentController(GLComponentController *glComponentController);
    const std::string &getLastError() const;
    const std::vector<std::string> getAlreadyUsedNPCIds() const;
    const std::vector<mapeditor::controllers::NPCDTO> getNPCs() const;
    const std::optional<mapeditor::controllers::NPCDTO> getNPCById(const std::string &name) const;
    const std::string &getEditedId() const;
    void setEditedId(const std::string &id);
 private:
    GLComponentController *m_glComponentController = nullptr;
    std::string m_lastError = "";
    std::string m_editedId = "";
};

}  // namespace mapeditor::controllers

