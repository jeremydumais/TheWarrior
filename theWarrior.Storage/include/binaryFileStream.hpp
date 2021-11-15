#pragma once

#include "iBinaryFileStream.hpp"
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>

template<class T>
class BinaryFileStream : public IBinaryFileStream<T>
{
public:
    BinaryFileStream(const std::string &fileName)
        : IBinaryFileStream<T>(fileName) {}

    bool open() override {
        try {
            m_ifs.exceptions(std::ifstream::failbit);
            m_ifs.open(this->getFileName(), std::ifstream::binary);
        }
        catch(const std::ios_base::failure &fail) {
            this->setLastError(fail.what());
            return false;
        }
        return true;
    }

    bool close() override {
        try {
            m_ifs.close();
        }
        catch(const std::ios_base::failure &fail) {
            this->setLastError(fail.what());
            return false;
        }
        return true;
    }

    bool readAllInto(T &obj) override {
        try {
            boost::archive::binary_iarchive oa(m_ifs);
            oa >> obj;
        }
        catch(const std::ios_base::failure &fail) {
            this->setLastError(fail.what());
            return false;
        }
        return true;
    }
private:
    std::ifstream m_ifs;
};