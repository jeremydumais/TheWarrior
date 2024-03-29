#include "jsonFileStream.hpp"
#include <boost/filesystem.hpp>
#include <fmt/format.h>

using namespace std;
using namespace boost::filesystem;
using namespace boost::property_tree::json_parser;

namespace thewarrior::storage {

JSONFileStream::JSONFileStream(const std::string &fileName)
    : IJSONFileStream(fileName)
{
}

bool JSONFileStream::fileExists() const
{
    return exists(getFileName());
}

bool JSONFileStream::readFile(boost::property_tree::ptree &obj)
{
    try {
        read_json(getFileName(), obj);
    }
    catch(const json_parser_error &err) {
        setLastError(err.what());
        return false;
    }
    return true;
}

bool JSONFileStream::writeFile(const boost::property_tree::ptree &obj)
{
    try {
        write_json(getFileName(), obj);
        return true;
    }
    catch(const json_parser_error &err) {
        setLastError(err.what());
        return false;
    }
    return true;
}

} // namespace thewarrior::storage
