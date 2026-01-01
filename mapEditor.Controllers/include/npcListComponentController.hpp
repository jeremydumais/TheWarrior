#pragma once

#include <string>
#include <boost/optional/optional.hpp>
#include "glComponentController.hpp"

namespace mapeditor::controllers {

class NPCListComponentController {
 public:
    explicit NPCListComponentController(GLComponentController *glComponentController);
    const std::string &getLastError() const;
 private:
    GLComponentController *m_glComponentController = nullptr;
    std::string m_lastError = "";
};

}  // namespace mapeditor::controllers

