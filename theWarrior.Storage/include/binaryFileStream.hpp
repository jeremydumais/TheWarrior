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
            ifs.exceptions(std::ifstream::failbit);
            ifs.open(this->getFileName(), std::ifstream::binary);
        }
        catch(const std::ios_base::failure &fail) {
            this->setLastError(fail.what());
            return false;
        }
        return true;
    }

    bool close() override {
        try {
            ifs.close();
        }
        catch(const std::ios_base::failure &fail) {
            this->setLastError(fail.what());
            return false;
        }
        return true;
    }

    bool readAllInto(T &obj) override {
        try {
            boost::archive::binary_iarchive oa(ifs);
            oa >> obj;
        }
        catch(const std::ios_base::failure &fail) {
            this->setLastError(fail.what());
            return false;
        }
        return true;
    }
private:
    std::ifstream ifs;
};