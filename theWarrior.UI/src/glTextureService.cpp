#include "glTextureService.hpp"
#include <fmt/format.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

using namespace thewarrior::models;

namespace thewarrior::ui {

GLTextureService::GLTextureService()
    : m_resourcePath("")
{
}

void GLTextureService::setResourcesPath(const std::string &resourcesPath)
{
    m_resourcePath = resourcesPath;
}

void GLTextureService::loadTexture(GLTexture &glTexture)
{
    loadTexture(glTexture.texture, glTexture.glTextureId);
}

void GLTextureService::loadTexture(const Texture &texture, unsigned int &textureGLId)
{
    glGenTextures(1, &textureGLId);
    glBindTexture(GL_TEXTURE_2D, textureGLId);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    std::string texFileName { texture.getFilename() };
    std::string fullResourcePath = fmt::format("{0}/textures/{1}", m_resourcePath, texFileName);
    unsigned char *imageBytes = stbi_load(fullResourcePath.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
    if (imageBytes) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageBytes);
    }
    else {
        throw std::runtime_error(fmt::format("Failed to load texture {0}", fullResourcePath));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageBytes);
}

void GLTextureService::unloadTexture(unsigned int &textureGLId)
{
    glDeleteTextures(1, &textureGLId);
}

void GLTextureService::setTextureUVFromIndex(const Texture *texture, GLfloat uvMap[4][2], int index) const
{
    float indexTile { static_cast<float>(index) };
    const int NBTEXTUREPERLINE { texture->getWidth() / texture->getTileWidth() };
    float lineIndex = floor(indexTile / static_cast<float>(NBTEXTUREPERLINE));
    const float TEXTURETILEWIDTH { texture->getTileWidthGL() };
    const float TEXTURETILEHEIGHT { texture->getTileHeightGL() };
    const float TEXTUREWIDTHADJUSTMENT { TEXTURETILEWIDTH / 40.0f };
    const float TEXTUREHEIGHTADJUSTMENT { TEXTURETILEHEIGHT / 40.0f };
    GLfloat lowerLeftCorner { indexTile / static_cast<float>(NBTEXTUREPERLINE) - floor(indexTile / static_cast<float>(NBTEXTUREPERLINE))  };

    uvMap[0][0] = (lowerLeftCorner) + TEXTUREWIDTHADJUSTMENT;
    uvMap[0][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTURETILEHEIGHT + TEXTUREHEIGHTADJUSTMENT;
    uvMap[1][0] = lowerLeftCorner + TEXTURETILEWIDTH - TEXTUREWIDTHADJUSTMENT;
    uvMap[1][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTURETILEHEIGHT + TEXTUREHEIGHTADJUSTMENT;
    uvMap[2][0] = lowerLeftCorner + TEXTURETILEWIDTH - TEXTUREWIDTHADJUSTMENT;
    uvMap[2][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTUREHEIGHTADJUSTMENT;
    uvMap[3][0] = lowerLeftCorner + TEXTUREWIDTHADJUSTMENT;
    uvMap[3][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTUREHEIGHTADJUSTMENT;
}

} // namespace thewarrior::ui
