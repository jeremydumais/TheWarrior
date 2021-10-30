#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/property_tree/json_parser.hpp>
#include <string>

class IJSONFileStream
{
public:
    explicit IJSONFileStream(const std::string &fileName) 
        : fileName(fileName),
          lastError("") {}
    virtual ~IJSONFileStream() = default;
    IJSONFileStream(const IJSONFileStream &) = default;
    IJSONFileStream(IJSONFileStream &&) = default;
    IJSONFileStream &operator=(const IJSONFileStream &) = default;
    IJSONFileStream &operator=(IJSONFileStream &&) = default;
    const std::string &getFileName() const { return this->fileName; }
    virtual const std::string &getLastError() const { return this->lastError; }
    void setLastError(const std::string &lastError) { this->lastError = lastError; }
    virtual bool fileExists() const = 0;
    virtual bool readFile(boost::property_tree::ptree &obj) = 0;
    virtual bool writeFile(const boost::property_tree::ptree &obj) = 0;
private:
    std::string fileName;
    std::string lastError;
};