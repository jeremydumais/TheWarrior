#pragma once

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
 private:
    GLComponentController *m_glComponentController = nullptr;
    std::string m_lastError = "";
};

}  // namespace mapeditor::controllers

