#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

namespace thewarrior::ui {

class GLShaderProgram
{
public:
    GLShaderProgram(const std::string &vertexShaderFileName,
                    const std::string &fragmentShaderFileName);
    ~GLShaderProgram();
    const std::string &getLastError() const;
    GLuint getShaderProgramID() const;
    bool compileShaders();
    bool linkShaders(const std::vector<std::string> &attributes);
    void use();
private:
    std::string m_lastError;
    std::string m_vertexShaderFileName;
    std::string m_fragmentShaderFileName;
    GLuint m_vertexshader;
    GLuint m_fragmentshader;
    GLuint m_shaderprogram;
    std::string loadShaderFile(const std::string &file);
};

} // namespace thewarrior::ui
