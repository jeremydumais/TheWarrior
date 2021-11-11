#include "texture.hpp"
#include <boost/algorithm/string.hpp>
#include <stdexcept>

using namespace std;
using namespace boost::algorithm;

Texture::Texture(const TextureInfo &textureInfo)
    : m_name(textureInfo.name),
      m_filename(textureInfo.filename),
      m_width(textureInfo.width),
      m_height(textureInfo.height),
      m_tileWidth(textureInfo.tileWidth),
      m_tileHeight(textureInfo.tileHeight)

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
    return m_name;
}

const std::string &Texture::getFilename() const
{
    return m_filename;
}

void Texture::setName(const std::string &name) 
{
    if (trim_copy(name).empty()) {
        throw invalid_argument("name cannot be null or empty.");
    }    
    m_name = name;
}

void Texture::setFilename(const std::string &filename) 
{
    if (trim_copy(filename).empty()) {
        throw invalid_argument("filename cannot be null or empty.");
    } 
    m_filename = filename;  
}

int Texture::getWidth() const
{
    return m_width;
}

int Texture::getHeight() const
{
    return m_height;
}

int Texture::getTileWidth() const
{
    return m_tileWidth;
}

int Texture::getTileHeight() const
{
    return m_tileHeight;
}

float Texture::getTileWidthGL() const
{
    return m_tileWidthGL;
}

float Texture::getTileHeightGL() const
{
    return m_tileHeightGL;
}

TextureInfo Texture::getTextureInfo() const
{
    return TextureInfo {
        m_name,
        m_filename,
        m_width,
        m_height,
        m_tileWidth,
        m_tileHeight
    };
}

void Texture::setWidth(int value) 
{
    if (value <= 0) {
        throw invalid_argument("width must be greater than zero.");
    }
    if (value < m_tileWidth) {
        throw invalid_argument("tile width must be less than the width.");
    }
    m_width = value;
    updateTileWidthGL();
}

void Texture::setHeight(int value) 
{
    if (value <= 0) {
        throw invalid_argument("height must be greater than zero.");
    }
    if (value < m_tileHeight) {
        throw invalid_argument("tile height must be less than the height.");
    }
    m_height = value;
    updateTileHeightGL();

}

void Texture::setTileWidth(int value) 
{
    if (value <= 0) {
        throw invalid_argument("tile width must be greater than zero.");
    }
    if (value > m_width) {
        throw invalid_argument("tile width must be less than the width.");
    }
    m_tileWidth = value;
    updateTileWidthGL();
}

void Texture::setTileHeight(int value) 
{
    if (value <= 0) {
        throw invalid_argument("tile height must be greater than zero.");
    }
    if (value > m_height) {
        throw invalid_argument("tile height must be less than the height.");
    }
    m_tileHeight = value;
    updateTileHeightGL();
}

void Texture::updateTileWidthGL() 
{
    m_tileWidthGL = static_cast<float>(m_tileWidth) / static_cast<float>(m_width);
}

void Texture::updateTileHeightGL() 
{
    m_tileHeightGL = static_cast<float>(m_tileHeight) / static_cast<float>(m_height);
}
