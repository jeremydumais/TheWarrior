#include <fmt/format.h>
#include <cstdlib>
#include <string>
#include "specialFolders.hpp"
#include <boost/algorithm/string.hpp>
#include "constants.hpp"

namespace thewarrior::utils {

const std::string SpecialFolders::getAppConfigDirectory(const std::string &appConfigFolder) {
    #ifdef _WIN32
        return fmt::format("{0}\\{1}\\",
                           getenv("LOCALAPPDATA"),
                           appConfigFolder);
    #else
        // Linux system
        return fmt::format("{0}/.config/{1}",
                           getenv("HOME"),
                           appConfigFolder);
    #endif
}

const std::string SpecialFolders::getAppDataDirectory(const std::string &appDataFolder) {
    #ifdef _WIN32
        return fmt::format("{0}\\{1}\\",
                           getenv("LOCALAPPDATA"),
                           appDataFolder);
    #else
        // Linux system
        return fmt::format("{0}/.local/{1}",
                           getenv("HOME"),
                           appDataFolder);
    #endif
}

const std::string SpecialFolders::getSaveGameDirectory() {
    const std::string appDataFolder = fmt::format("{0}/{1}",
                                                  thewarrior::utils::ORGANIZATIONNAME,
                                                  thewarrior::utils::APPLICATIONNAME);
    #ifdef _WIN32
        return fmt::format("{0}\\Saves",
                           SpecialFolders::getAppDataDirectory(appDataFolder));
    #else
        // Linux system
        return fmt::format("{0}/Saves",
                           SpecialFolders::getAppDataDirectory(appDataFolder));
    #endif
}

}  // namespace thewarrior::utils
