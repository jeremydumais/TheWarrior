#pragma once

#include "openChestActionProperties.hpp"
#include <map>
#include <string>

namespace thewarrior::models {

class OpenChestActionPropertiesConverter {
 public:
    static OpenChestActionProperties fromMap(const std::map<std::string, std::string>& properties);
    static std::map<std::string, std::string> toMap(const OpenChestActionProperties& properties);
};

}  // namespace thewarrior::models