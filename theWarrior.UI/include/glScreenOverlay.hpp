#pragma once

#include <memory>
#include <string>
#include <GL/glew.h>
#include "glShaderProgram.hpp"

namespace thewarrior::ui {

class GLScreenOverlay {
 public:
    GLScreenOverlay() = default;
    ~GLScreenOverlay();

    bool initialize(const std::string &resourcesPath);
    void render(float opacity, float brightness = 0.0F) const;
    void unload();

    const std::string &getLastError() const;
 private:
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
    GLuint m_vao = 0;
    GLuint m_positionVbo = 0;

    GLint m_colorUniform = -1;
    GLint m_opacityUniform = -1;

    std::string m_lastError;
};

}  // namespace thewarrior::ui