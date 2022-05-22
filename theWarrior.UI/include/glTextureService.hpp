#pragma once

#include "texture.hpp"
#include <GL/glew.h>
#include <string>

class GLTextureService
{
public:
    GLTextureService();
    void setResourcesPath(const std::string &resourcesPath);
    void loadTexture(const Texture &texture, unsigned int &textureGLId);
    void unloadTexture(unsigned int &textureGLId);
    void setTextureUVFromIndex(const Texture *texture, GLfloat uvMap[4][2], int index) const;
private:
    std::string m_resourcePath;
};