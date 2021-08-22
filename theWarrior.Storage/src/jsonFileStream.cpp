#include "jsonFileStream.hpp"


using namespace std;
using namespace boost::property_tree::json_parser;

JSONFileStream::JSONFileStream(const std::string &fileName) 
    : IJSONFileStream(fileName)
{
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
