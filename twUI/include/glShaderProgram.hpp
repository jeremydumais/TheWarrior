#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

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
    std::string lastError;
    std::string vertexShaderFileName;
    std::string fragmentShaderFileName;
    GLuint vertexshader;
    GLuint fragmentshader;
    GLuint shaderprogram;
    std::string loadShaderFile(const std::string &file);
};