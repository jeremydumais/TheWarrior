#include "glTileService.hpp"

using namespace std;


bool GLTileService::initShader(const std::string &vertexShaderFileName,
                         const std::string &fragmentShaderFileName) 
{
    shaderProgram = make_unique<GLShaderProgram>(vertexShaderFileName,
                                                 fragmentShaderFileName);
    if (!shaderProgram->compileShaders()) {
        lastError = shaderProgram->getLastError();
        return false;
    }
    if (!shaderProgram->linkShaders({ "in_Position", "in_Color", "in_VertexUV" })) {
        lastError = shaderProgram->getLastError();
        return false;
    }
    return true;
}

const string& GLTileService::getLastError() const
{
    return lastError;
}

void GLTileService::useShader() 
{
    shaderProgram->use();
}

void GLTileService::setShaderTranslation(unsigned int mapWidth, unsigned int mapHeight, int windowWidth, int windowHeight, const GLPlayer &glPlayer) 
{
    int vertexTranslationLocation = glGetUniformLocation(shaderProgram->getShaderProgramID(), "translation");
    float mapMiddleH { 0.0f };
    float mapMiddleV { 0.0f };
    //If the map width is smaller than the window width
    if (static_cast<float>(mapWidth) * 51.2F <= static_cast<float>(windowWidth)) {
        //Center target: map
        mapMiddleH = 1.0F - ((static_cast<float>(mapWidth) * 51.2F) / static_cast<float>(windowWidth));
    }
    else {
        //Center target: player
        mapMiddleH = 1.0f - (((static_cast<float>(glPlayer.coord.x()) + glPlayer.xMove) * 2.0f * 51.2f) / static_cast<float>(windowWidth));
    }
    //If the map height is smaller than the window height
    if (static_cast<float>(mapHeight) * 51.2F <= static_cast<float>(windowHeight)) {
        //Center target: map
        mapMiddleV = (1.0f - (static_cast<float>(mapHeight) * 51.2f) / static_cast<float>(windowHeight)) * -1.0f;
    }
    else {
        //Center target: player
        mapMiddleV = (1.0f - ((static_cast<float>(glPlayer.coord.y()) + glPlayer.yMove) * 2.0f * 51.2f) / static_cast<float>(windowHeight)) * -1.0f;
    }
    glUniform2f(vertexTranslationLocation, mapMiddleH, mapMiddleV);
    GLint TextureID  = glGetUniformLocation(shaderProgram->getShaderProgramID(), "myTextureSampler");
    glUniform1i(TextureID, 0);
}
