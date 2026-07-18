#pragma once

#include <GL/glew.h>
#include "texture.hpp"

namespace thewarrior::ui {

struct GLObject {
    GLuint vao = 0;
    GLuint vboPosition = 0;
    GLuint vboColor = 0;
    GLuint vboTexture = 0;
    GLuint textureGLId = 0;
};

struct GenerateGLObjectInfo {
    GLObject *glObject;
    const thewarrior::models::Texture *texture;
    const int textureIndex;
    GLuint *specialVAO = nullptr;
    GLuint *specialVBOTexture = nullptr;
};

class GLObjectService {
 public:
    static void generateGLObject(GenerateGLObjectInfo &info, const GLfloat tileCoord[4][2], const GLfloat colors[4][3]);
};

}  // namespace thewarrior::ui
