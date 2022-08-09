#pragma once

#include "iJSONFileStream.hpp"

namespace thewarrior::storage {

class JSONFileStream : public IJSONFileStream
{
public:
    explicit JSONFileStream(const std::string &fileName);
    bool fileExists() const override;
    bool readFile(boost::property_tree::ptree &obj) override;
    bool writeFile(const boost::property_tree::ptree &obj) override;
};

} // namespace thewarrior::storage
