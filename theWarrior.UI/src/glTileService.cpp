#include "glTileService.hpp"

using namespace std;


bool GLTileService::initShader(const std::string &vertexShaderFileName,
                         const std::string &fragmentShaderFileName) 
{
    m_shaderProgram = make_shared<GLShaderProgram>(vertexShaderFileName,
                                                 fragmentShaderFileName);
    if (!m_shaderProgram->compileShaders()) {
        m_lastError = m_shaderProgram->getLastError();
        return false;
    }
    if (!m_shaderProgram->linkShaders({ "in_Position", "in_Color", "in_VertexUV" })) {
        m_lastError = m_shaderProgram->getLastError();
        return false;
    }
    return true;
}

void GLTileService::initShader(const std::shared_ptr<GLShaderProgram> shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

const string& GLTileService::getLastError() const
{
    return m_lastError;
}

void GLTileService::useShader() 
{
    m_shaderProgram->use();
}

void GLTileService::setShaderTranslation(unsigned int mapWidth, unsigned int mapHeight, int windowWidth, int windowHeight, Point<float> playerPosition) 
{
    int vertexTranslationLocation = glGetUniformLocation(m_shaderProgram->getShaderProgramID(), "translation");
    float mapMiddleH { 0.0f };
    float mapMiddleV { 0.0f };
    //If the map width is smaller than the window width
    if (static_cast<float>(mapWidth) * 51.2F <= static_cast<float>(windowWidth)) {
        //Center target: map
        mapMiddleH = 1.0F - ((static_cast<float>(mapWidth) * 51.2F) / static_cast<float>(windowWidth));
    }
    else {
        //Center target: player
        mapMiddleH = 1.0f - ((playerPosition.x() * 2.0f * 51.2f) / static_cast<float>(windowWidth));
    }
    //If the map height is smaller than the window height
    if (static_cast<float>(mapHeight) * 51.2F <= static_cast<float>(windowHeight)) {
        //Center target: map
        mapMiddleV = (1.0f - (static_cast<float>(mapHeight) * 51.2f) / static_cast<float>(windowHeight)) * -1.0f;
    }
    else {
        //Center target: player
        mapMiddleV = (1.0f - (playerPosition.y() * 2.0f * 51.2f) / static_cast<float>(windowHeight)) * -1.0f;
    }
    glUniform2f(vertexTranslationLocation, mapMiddleH, mapMiddleV);
    GLint TextureID  = glGetUniformLocation(m_shaderProgram->getShaderProgramID(), "myTextureSampler");
    glUniform1i(TextureID, 0);
}
