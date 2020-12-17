#include "texture.hpp"
#include <boost/algorithm/string.hpp>
#include <stdexcept>

using namespace std;
using namespace boost::algorithm;

Texture::Texture(const std::string &name, const std::string &filename)
    : name(name),
      filename(filename)
{
    if (trim_copy(name).empty()) {
        throw invalid_argument("name cannot be null or empty.");
    }
    if (trim_copy(filename).empty()) {
        throw invalid_argument("filename cannot be null or empty.");
    }
}

const std::string &Texture::getName() const
{
    return name;
}

const std::string &Texture::getFilename() const
{
    return filename;
}

void Texture::setName(const std::string &name) 
{
    if (trim_copy(name).empty()) {
        throw invalid_argument("name cannot be null or empty.");
    }    
    this->name = name;
}

void Texture::setFilename(const std::string &filename) 
{
    if (trim_copy(filename).empty()) {
        throw invalid_argument("filename cannot be null or empty.");
    } 
    this->filename = filename;  
}
