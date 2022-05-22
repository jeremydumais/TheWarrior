#pragma once

#include "glShaderProgram.hpp"
#include "IShaderService.hpp"
#include "point.hpp"
#include <memory>
#include <string>

class GLTileService : public IShaderService
{
public:
    bool initShader(const std::string &vertexShaderFileName,
              const std::string &fragmentShaderFileName) override;
    const std::string &getLastError() const;
    void useShader();
    void setShaderTranslation(unsigned int mapWidth, unsigned int mapHeight, int windowWidth, int windowHeight, Point<float> playerPosition);
private:
    std::string lastError;
    std::unique_ptr<GLShaderProgram> shaderProgram;
};