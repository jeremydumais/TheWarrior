#include "texture.hpp"
#include <boost/algorithm/string.hpp>
#include <stdexcept>

using namespace std;
using namespace boost::algorithm;

Texture::Texture(const TextureInfo &textureInfo)
    : name(textureInfo.name),
      filename(textureInfo.filename),
      width(textureInfo.width),
      height(textureInfo.height),
      tileWidth(textureInfo.tileWidth),
      tileHeight(textureInfo.tileHeight)

{
    if (trim_copy(textureInfo.name).empty()) {
        throw invalid_argument("name cannot be null or empty.");
    }
    if (trim_copy(textureInfo.filename).empty()) {
        throw invalid_argument("filename cannot be null or empty.");
    }
    if (textureInfo.width <= 0) {
        throw invalid_argument("width must be greater than zero.");
    }
    if (textureInfo.height <= 0) {
        throw invalid_argument("height must be greater than zero.");
    }
    if (textureInfo.tileWidth <= 0) {
        throw invalid_argument("tile width must be greater than zero.");
    }
    if (textureInfo.tileHeight <= 0) {
        throw invalid_argument("tile height must be greater than zero.");
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

int Texture::getWidth() const
{
    return width;
}

int Texture::getHeight() const
{
    return height;
}

int Texture::getTileWidth() const
{
    return tileWidth;
}

int Texture::getTileHeight() const
{
    return tileHeight;
}

void Texture::setWidth(int value) 
{
    //TODO complete method and tests
}

void Texture::setHeight(int value) 
{
    //TODO complete method and tests
}

void Texture::setTileWidth(int value) 
{
    //TODO complete method and tests
}

void Texture::setTileHeight(int value) 
{
    //TODO complete method and tests
}
