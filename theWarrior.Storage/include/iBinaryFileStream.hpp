#pragma once

#include <string>

enum class FileOpenMode { Read, Write };

template<class T>
class IBinaryFileStream
{
public:   
    IBinaryFileStream(const std::string &fileName) : m_fileName(fileName) {}
    virtual ~IBinaryFileStream() = default;
    const std::string &getFileName() const { return m_fileName; }
    void setLastError(const std::string &lastError) { m_lastError = lastError; }
    virtual bool open(FileOpenMode mode) = 0;
    virtual bool close() = 0;
    virtual bool readAllInto(T &obj) = 0;
    virtual bool write(const T &obj) = 0;
private:
    std::string m_fileName;
    std::string m_lastError;
};