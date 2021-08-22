#pragma once

#include <string>

class SpecialFolders
{
public:
    static const std::string getAppConfigDirectory(const std::string &appName);
};