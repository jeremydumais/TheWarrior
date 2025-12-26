#pragma once

#include <string>
#include "../../theWarrior.Storage/include/iGameStateRepository.hpp"

namespace thewarrior::ui::controllers {

class LoadGameScreenController {
 public:
     LoadGameScreenController();
     const std::string &getLastError() const;
     storage::GameStateRepositoryListResult getGameStateList();

 private:
     std::string m_lastError;
};

}  // namespace thewarrior::ui::controllers

