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
    //Ensure that the tile width is less than width
    if (textureInfo.width < textureInfo.tileWidth) {
        throw invalid_argument("tile width must be less than the width.");
    }
    //Ensure that the tile height is less than height
    if (textureInfo.height < textureInfo.tileHeight) {
        throw invalid_argument("tile height must be less than the height.");
    }
    updateTileWidthGL();
    updateTileHeightGL();
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

float Texture::getTileWidthGL() const
{
    return tileWidthGL;
}

float Texture::getTileHeightGL() const
{
    return tileHeightGL;
}

TextureInfo Texture::getTextureInfo() const
{
    return TextureInfo {
        name,
        filename,
        width,
        height,
        tileWidth,
        tileHeight
    };
}

void Texture::setWidth(int value) 
{
    if (value <= 0) {
        throw invalid_argument("width must be greater than zero.");
    }
    if (value < this->tileWidth) {
        throw invalid_argument("tile width must be less than the width.");
    }
    this->width = value;
    updateTileWidthGL();
}

void Texture::setHeight(int value) 
{
    if (value <= 0) {
        throw invalid_argument("height must be greater than zero.");
    }
    if (value < this->tileHeight) {
        throw invalid_argument("tile height must be less than the height.");
    }
    this->height = value;
    updateTileHeightGL();

}

void Texture::setTileWidth(int value) 
{
    if (value <= 0) {
        throw invalid_argument("tile width must be greater than zero.");
    }
    if (value > this->width) {
        throw invalid_argument("tile width must be less than the width.");
    }
    this->tileWidth = value;
    updateTileWidthGL();
}

void Texture::setTileHeight(int value) 
{
    if (value <= 0) {
        throw invalid_argument("tile height must be greater than zero.");
    }
    if (value > this->height) {
        throw invalid_argument("tile height must be less than the height.");
    }
    this->tileHeight = value;
    updateTileHeightGL();
}

void Texture::updateTileWidthGL() 
{
    this->tileWidthGL = static_cast<float>(this->tileWidth) / static_cast<float>(this->width);
}

void Texture::updateTileHeightGL() 
{
    this->tileHeightGL = static_cast<float>(this->tileHeight) / static_cast<float>(this->height);
}
