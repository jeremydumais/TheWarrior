#include "specialFolders.hpp"
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <fmt/format.h>

using namespace std;

namespace thewarrior::storage {

const string SpecialFolders::getAppConfigDirectory(const string &appConfigFolder)
{
    #ifdef _WIN32
        return fmt::format("{0}\\{1}\\",
                           getenv("LOCALAPPDATA"),
                           appConfigFolder);
    #else
        //Linux system
        return fmt::format("{0}/.config/{1}/",
                           getenv("HOME"),
                           appConfigFolder);
    #endif

}

} // namespace thewarrior::storage
