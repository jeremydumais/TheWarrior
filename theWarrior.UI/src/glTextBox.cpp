#include "glTextBox.hpp"
#include "itemFoundMessageDTO.hpp"
#include <fmt/format.h>
#include <stdexcept>

GLTextBox::GLTextBox()
    : m_itemStore(nullptr),
      m_lastError(""),
      m_messageDTO(nullptr)
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
void GLTextBox::setItemStore(std::shared_ptr<ItemStore> itemStore)
{
    m_itemStore = itemStore;
}

void GLTextBox::setItemStoreTextureMap(const std::map<std::string, unsigned int> *texturesGLItemStore)
{
    m_texturesGLItemStore = texturesGLItemStore;
}

void GLTextBox::generateMessage(std::shared_ptr<MessageDTO> messageDTO, int screenWidth, int screenHeight)
{
    m_messageDTO = messageDTO;
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
    if (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) {
        //Find the icon
        ItemFoundMessageDTO *itemFoundMsgDTO = dynamic_cast<ItemFoundMessageDTO *>(messageDTO.get());
        auto item = m_itemStore->findItem(itemFoundMsgDTO->itemId);
        if (!item) {
            throw std::runtime_error(fmt::format("Unable to found the item {0}", itemFoundMsgDTO->itemId));
        }
        //Find the texture
        auto texture = m_itemStore->getTextureContainer().getTextureByName(item->getTextureName());
        if (!texture.has_value()) {
            throw std::runtime_error(fmt::format("Unable to found the texture {0}", item->getTextureName()));
        }

        float screenWidthFloat = static_cast<float>(screenWidth);
        float screenHeightFloat = static_cast<float>(screenHeight);
        float ratioHeight = screenWidthFloat / screenHeightFloat;

        const float BOXWIDTH = 0.05F * (1400.0f / screenWidthFloat);
        const float BOXHEIGHT = BOXWIDTH * ratioHeight;
        GLfloat tileCoordIcon[4][2] {
        { -BOXWIDTH + startPosX,  BOXHEIGHT + startPosY - 0.06F },     /* Top Left point */
        {  BOXWIDTH + startPosX,  BOXHEIGHT + startPosY - 0.06F },     /* Top Right point */
        {  BOXWIDTH + startPosX, -BOXHEIGHT + startPosY - 0.06F },     /* Bottom Right point */
        { -BOXWIDTH + startPosX, -BOXHEIGHT + startPosY - 0.06F } };   /* Bottom Left point */

        GenerateGLObjectInfo infoGenTextureIcon {
            &m_glObjectIcon,
            &texture.value().get(),
            item->getTextureIndex()};
        GLObjectService::generateGLObject(infoGenTextureIcon, tileCoordIcon, texColorBuf);
    }
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
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
    if (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) {
        ItemFoundMessageDTO *dto = dynamic_cast<ItemFoundMessageDTO *>(m_messageDTO.get());
        //Display the icon
        glBindTexture(GL_TEXTURE_2D, (*m_texturesGLItemStore).at(dto->textureName));
        glBindVertexArray(m_glObjectIcon.vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_glObjectIcon.vboPosition);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_glObjectIcon.vboColor);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, m_glObjectIcon.vboTexture);
        glEnableVertexAttribArray(2);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }
    textService.useShader();
    const float SCALE = 0.7F;
    float messageWidth = textService.getTextWidth(m_messageDTO->message, SCALE);
    textService.renderText(m_messageDTO->message, 
                        (static_cast<float>(screenWidth) / 2.0F) - (messageWidth / 2.0F),                             // X
                        static_cast<float>(screenHeight) / 2.0F + 20.0f,                             // Y
                        SCALE,                               // Scale
                        glm::vec3(1.0f, 1.0f, 1.0f));       // Color

}