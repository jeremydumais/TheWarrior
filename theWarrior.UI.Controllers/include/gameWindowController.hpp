#pragma once

#include <string>

namespace thewarrior::ui::controllers {

class GameWindowController {
 public:
    GameWindowController();
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;

 private:
    std::string m_resourcesPath;
    std::string m_lastError;
    void initializeResourcesPath();
};

}  // namespace thewarrior::ui::controllers
