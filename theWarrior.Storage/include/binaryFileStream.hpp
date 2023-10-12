#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include "iBinaryFileStream.hpp"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/archive_exception.hpp>

namespace thewarrior::storage {

template<class T>
class BinaryFileStream : public IBinaryFileStream<T> {
 public:
    explicit BinaryFileStream(const std::string &fileName)
        : IBinaryFileStream<T>(fileName) {}

    bool open(FileOpenMode mode) override {
        try {
            m_fs.exceptions(std::fstream::failbit);
            m_fs.open(this->getFileName(),
                      std::fstream::binary |
                      (mode == FileOpenMode::Read ? std::fstream::in : std::fstream::out));
        }
        catch(const std::ios_base::failure &fail) {
            this->setLastError(fail.what());
            return false;
        }
        return true;
    }

    bool close() override {
        try {
            m_fs.close();
        }
        catch(const std::ios_base::failure &fail) {
            this->setLastError(fail.what());
            return false;
        }
        return true;
    }

    bool readAllInto(T &obj) override {
        try {
            boost::archive::binary_iarchive oa(m_fs);
            oa >> obj;
        } catch (const boost::archive::archive_exception& e) {
            this->setLastError(e.what());
            return false;
        } catch(const std::ios_base::failure &fail) {
            this->setLastError(fail.what());
            return false;
        } catch (const std::exception& e) {
            this->setLastError(e.what());
            return false;
        }
        return true;
    }

    bool write(const T &obj) override {
        try {
            boost::archive::binary_oarchive oa(m_fs);
            oa << obj;
        }
        catch(const std::ios_base::failure &fail) {
            this->setLastError(fail.what());
            return false;
        } catch (const std::exception& e) {
            this->setLastError(e.what());
            return false;
        }
        return true;
    }

 private:
    std::fstream m_fs;
};

}  // namespace thewarrior::storage
