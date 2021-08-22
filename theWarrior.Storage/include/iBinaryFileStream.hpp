#pragma once

#include <string>

template<class T>
class IBinaryFileStream
{
public:
    IBinaryFileStream(const std::string &fileName) : fileName(fileName) {}
    virtual ~IBinaryFileStream() = default;
    const std::string &getFileName() const { return this->fileName; }
    void setLastError(const std::string &lastError) { this->lastError = lastError; }
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool readAllInto(T &obj) = 0;
private:
    std::string fileName;
    std::string lastError;
};