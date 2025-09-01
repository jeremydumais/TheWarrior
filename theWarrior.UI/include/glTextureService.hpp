#pragma once

#include <GL/glew.h>
#include <string>
#include "glTexture.hpp"
#include "texture.hpp"

namespace thewarrior::ui {

class GLTextureService {
 public:
    GLTextureService();
    void setResourcesPath(const std::string &resourcesPath);
    void loadTexture(GLTexture &glTexture);
    void loadTexture(const thewarrior::models::Texture &texture, unsigned int &textureGLId);
    void unloadTexture(unsigned int &textureGLId);
    void setTextureUVFromIndex(const thewarrior::models::Texture *texture,
                               GLfloat uvMap[4][2],
                               int index) const;

 private:
    std::string m_resourcePath;
};

}  // namespace thewarrior::ui
