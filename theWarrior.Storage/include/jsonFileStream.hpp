#pragma once

#include "iJSONFileStream.hpp"

class JSONFileStream : public IJSONFileStream
{
public:
    JSONFileStream(const std::string &fileName);
    bool readFile(boost::property_tree::ptree &obj) override;


};