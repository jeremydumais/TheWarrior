#include "glTextBox.hpp"
    
GLTextBox::GLTextBox()
    : m_lastError(""),
      m_message("")
{
}

bool GLTextBox::initShader(const std::string &vertexShaderFileName,
                         const std::string &fragmentShaderFileName) 
{
    shaderProgram = std::make_unique<GLShaderProgram>(vertexShaderFileName,
                                                 fragmentShaderFileName);
    if (!shaderProgram->compileShaders()) {
        m_lastError = shaderProgram->getLastError();
        return false;
    }
    if (!shaderProgram->linkShaders({ "vertex" })) {
        m_lastError = shaderProgram->getLastError();
        return false;
    }
    return true;
}

const std::string &GLTextBox::getLastError() const
{
    return m_lastError;
}

void GLTextBox::generateMessage(std::shared_ptr<MessageDTO> messageDTO)
{
    m_message = messageDTO->message;
    GLfloat texColorBuf[4][3] { { 1.0F, 1.0F, 1.0F },   /* Red */
                                { 1.0F, 1.0F, 1.0F },   /* Green */
                                { 1.0F, 1.0F, 1.0F },   /* Blue */
                                { 1.0F, 1.0F, 1.0F } };
    constexpr float BOXHALFWIDTH = 1.0F / 2.0F;
    constexpr float BOXHALFHEIGHT = 0.5F / 2.0F;
    float startPosX { 0.0F };
    float startPosY { 1.0F - (BOXHALFHEIGHT * 4.0F)};

    GLfloat tileCoord[4][2] = {
    { -BOXHALFWIDTH + startPosX,  BOXHALFHEIGHT + startPosY },     /* Top Left point */
    {  BOXHALFWIDTH + startPosX,  BOXHALFHEIGHT + startPosY },     /* Top Right point */
    {  BOXHALFWIDTH + startPosX, -BOXHALFHEIGHT + startPosY },     /* Bottom Right point */
    { -BOXHALFWIDTH + startPosX, -BOXHALFHEIGHT + startPosY } };   /* Bottom Left point */
    
    GenerateGLObjectInfo infoGenTexture {
            &m_glObject,
            nullptr,
            -1};
    GLObjectService::generateGLObject(infoGenTexture, tileCoord, texColorBuf);
}

void GLTextBox::useShader()
{
    shaderProgram->use();
}

void GLTextBox::draw(GLTextService &textService, int screenWidth, int screenHeight)
{
    useShader();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(m_glObject.vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_glObject.vboPosition);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_glObject.vboColor);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    textService.useShader();
    const float SCALE = 0.7F;
    float messageWidth = textService.getTextWidth(m_message, SCALE);
    textService.renderText(m_message, 
                        (static_cast<float>(screenWidth) / 2.0F) - (messageWidth / 2.0F),                             // X
                        static_cast<float>(screenHeight) / 2.0F,                             // Y
                        SCALE,                               // Scale
                        glm::vec3(1.0f, 1.0f, 1.0f));       // Color

}