#include <fmt/format.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>
#include <string>
#include "glTextureService.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui {

GLTextureService::GLTextureService()
    : m_resourcePath("") {
}

void GLTextureService::setResourcesPath(const std::string &resourcesPath) {
    m_resourcePath = resourcesPath;
}

void GLTextureService::loadTexture(GLTexture &glTexture) {
    loadTexture(glTexture.texture, glTexture.glTextureId);
}

void GLTextureService::loadTexture(const Texture &texture, unsigned int &textureGLId) {
    glGenTextures(1, &textureGLId);
    glBindTexture(GL_TEXTURE_2D, textureGLId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Preserve pixel-art edges without blending adjacent atlas tiles.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    std::string texFileName { texture.getFilename() };
    std::string fullResourcePath = fmt::format("{0}/textures/{1}", m_resourcePath, texFileName);
    unsigned char *imageBytes = stbi_load(fullResourcePath.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
    if (imageBytes) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageBytes);
    } else {
        throw std::runtime_error(fmt::format("Failed to load texture {0}", fullResourcePath));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageBytes);
}

void GLTextureService::unloadTexture(unsigned int &textureGLId) {
    glDeleteTextures(1, &textureGLId);
}

void GLTextureService::setTextureUVFromIndex(const Texture *texture, GLfloat uvMap[4][2], int index) const {
    const int tilesPerRow = texture->getWidth() / texture->getTileWidth();
    const int column = index % tilesPerRow;
    const int row = index / tilesPerRow;
    const float width = static_cast<float>(texture->getWidth());
    const float height = static_cast<float>(texture->getHeight());
    // Use the complete tile: an inset crops edge pixels and breaks tile seams.
    const float left = static_cast<float>(column * texture->getTileWidth()) / width;
    const float right = static_cast<float>((column + 1) * texture->getTileWidth()) / width;
    const float top = 1.0f - static_cast<float>(row * texture->getTileHeight()) / height;
    const float bottom = 1.0f - static_cast<float>((row + 1) * texture->getTileHeight()) / height;

    uvMap[0][0] = left;
    uvMap[0][1] = bottom;
    uvMap[1][0] = right;
    uvMap[1][1] = bottom;
    uvMap[2][0] = right;
    uvMap[2][1] = top;
    uvMap[3][0] = left;
    uvMap[3][1] = top;
}

}  // namespace thewarrior::ui
