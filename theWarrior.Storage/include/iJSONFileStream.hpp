#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <string>
#include <boost/property_tree/json_parser.hpp>


class IJSONFileStream
{
public:
    IJSONFileStream(const std::string &fileName) : fileName(fileName) {}
    virtual ~IJSONFileStream() = default;
    const std::string &getFileName() const { return this->fileName; }
    void setLastError(const std::string &lastError) { this->lastError = lastError; }
    virtual bool readFile(boost::property_tree::ptree &obj) = 0;
private:
    std::string fileName;
    std::string lastError;
};