#pragma once

#include <string>
#include <string_view>

namespace thewarrior::ui::controllers {

class NewGamePlayerNameScreenController {
 public:
    struct HasGameStateResult {
        bool success;
        bool hasGameState;
    };
    NewGamePlayerNameScreenController();
    const std::string &getLastError() const;
    bool hasAtLeastTwoAlphaAscii(std::string_view value);
    HasGameStateResult hasGameStatesForPlayer(std::string_view playerName);

 private:
     std::string m_lastError;
};

}  // namespace thewarrior::ui::controllers

