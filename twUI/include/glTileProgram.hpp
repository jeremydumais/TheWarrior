#pragma once

#include "glShaderProgram.hpp"
#include "glPlayer.hpp"
#include <memory>
#include <string>

class GLTileProgram
{
public:
    bool init(const std::string &vertexShaderFileName,
              const std::string &fragmentShaderFileName);
    const std::string &getLastError() const;
    void useShader();
    void setShaderTranslation(unsigned int mapWidth, unsigned int mapHeight, int windowWidth, int windowHeight, const GLPlayer &glPlayer);
private:
    std::string lastError;
    std::unique_ptr<GLShaderProgram> shaderProgram;
};