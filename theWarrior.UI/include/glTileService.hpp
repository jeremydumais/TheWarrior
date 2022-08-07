#pragma once

#include "glShaderProgram.hpp"
#include "IShaderService.hpp"
#include "point.hpp"
#include <memory>
#include <string>

namespace thewarrior::ui {

class GLTileService : public IShaderService
{
public:
    bool initShader(const std::string &vertexShaderFileName,
                    const std::string &fragmentShaderFileName) override;
    void initShader(const std::shared_ptr<GLShaderProgram> shaderProgram) override;
    const std::string &getLastError() const;
    void useShader();
    void setShaderTranslation(unsigned int mapWidth,
                              unsigned int mapHeight,
                              int windowWidth,
                              int windowHeight,
                              Point<float> playerPosition);
private:
    std::string m_lastError;
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
};

} // namespace thewarrior::ui
