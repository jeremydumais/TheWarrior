#include "glTextBox.hpp"
#include "itemFoundMessageDTO.hpp"
#include "point.hpp"
#include <fmt/format.h>
#include <stdexcept>

GLTextBox::GLTextBox()
    : m_screenSize(1.0F, 1.0F),
      m_textService(nullptr),
      m_itemStore(nullptr),
      m_lastError(""),
      m_messageDTO(nullptr),
      m_computedTextForDisplay({Size(0.0F, 0.0F), {}})
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

void GLTextBox::setScreenSize(Size<float> size)
{
    m_screenSize = size;
    //Resize currently displayed message
    if (m_messageDTO ) {
        generateMessage(m_messageDTO);
    }
}

void GLTextBox::setTextService(GLTextService *textService)
{
    m_textService = textService;
}

void GLTextBox::setItemStore(std::shared_ptr<ItemStore> itemStore)
{
    m_itemStore = itemStore;
}


void GLTextBox::setItemStoreTextureMap(const std::map<std::string, unsigned int> *texturesGLItemStore)
{
    m_texturesGLItemStore = texturesGLItemStore;
}

void GLTextBox::generateMessage(std::shared_ptr<MessageDTO> messageDTO)
{
    m_messageDTO = messageDTO;
    m_computedTextForDisplay = m_textService->prepareTextForDisplay(m_screenSize, m_messageDTO->message, messageDTO->scale);
    generateGLTextBox();
    if (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) {
        //Find the icon
        generateGLIcon();
    }
}

void GLTextBox::useShader()
{
    shaderProgram->use();
}

void GLTextBox::draw()
{
    useShader();
    drawQuad(m_glObject, 0);
    if (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) {
        //Display the icon
        ItemFoundMessageDTO *dto = dynamic_cast<ItemFoundMessageDTO *>(m_messageDTO.get());
        drawQuad(m_glObjectIcon, (*m_texturesGLItemStore).at(dto->textureName));
    }
    m_textService->useShader();
    float lineTotal = static_cast<float>(m_computedTextForDisplay.lines.size());
    float lineHeight = (m_computedTextForDisplay.textSize.height() / lineTotal) - 10.0F;
     
    Point<float> messagePosition((m_screenSize.width() / 2.0F) - (m_computedTextForDisplay.textSize.width() / 2.0F),
                                 (m_screenSize.height() / 2.0F) + ((lineTotal) * lineHeight) - 30.0F);
    if (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) {
        messagePosition.setY(messagePosition.y() + 20.0F);
    }
    for(size_t i = 0; i < m_computedTextForDisplay.lines.size(); i++) {
        m_textService->renderText(m_computedTextForDisplay.lines[i], 
                                  messagePosition.x(),
                                  messagePosition.y() - (static_cast<float>(i) * (lineHeight + 10.0F)),
                                  m_messageDTO->scale,
                                  glm::vec3(1.0f, 1.0f, 1.0f));       // Color
    }
}

void GLTextBox::generateGLTextBox()
{
    GLfloat texColorBuf[4][3];
    const float BOXHALFWIDTH = (m_computedTextForDisplay.textSize.width() / m_screenSize.width()) + (getComputedBoxPadding() / 2.0F);
    const float BOXHALFHEIGHT = getBoxHalfHeight();
    const Point<float> boxStartPosition = getBoxStartPosition();

    GLfloat tileCoord[4][2] = {
    { -BOXHALFWIDTH + boxStartPosition.x(),  BOXHALFHEIGHT + boxStartPosition.y() },     /* Top Left point */
    {  BOXHALFWIDTH + boxStartPosition.x(),  BOXHALFHEIGHT + boxStartPosition.y() },     /* Top Right point */
    {  BOXHALFWIDTH + boxStartPosition.x(), -BOXHALFHEIGHT + boxStartPosition.y() },     /* Bottom Right point */
    { -BOXHALFWIDTH + boxStartPosition.x(), -BOXHALFHEIGHT + boxStartPosition.y() } };   /* Bottom Left point */
    
    GenerateGLObjectInfo infoGenTexture {
            &m_glObject,
            nullptr,
            -1};
    GLObjectService::generateGLObject(infoGenTexture, tileCoord, texColorBuf);
}

void GLTextBox::generateGLIcon()
{
    GLfloat texColorBuf[4][3];
    const Point<float> boxStartPosition = getBoxStartPosition();
    ItemFoundMessageDTO *itemFoundMsgDTO = dynamic_cast<ItemFoundMessageDTO *>(m_messageDTO.get());
    auto item = m_itemStore->findItem(itemFoundMsgDTO->itemId);
    if (!item) {
        throw std::runtime_error(fmt::format("Unable to found the item {0}", itemFoundMsgDTO->itemId));
    }
    //Find the texture
    auto texture = m_itemStore->getTextureContainer().getTextureByName(item->getTextureName());
    if (!texture.has_value()) {
        throw std::runtime_error(fmt::format("Unable to found the texture {0}", item->getTextureName()));
    }

    float ratioHeight = m_screenSize.width() / m_screenSize.height();

    const float BOXWIDTH = 0.05F * (1400.0f / m_screenSize.width());
    const float BOXHEIGHT = BOXWIDTH * ratioHeight;
    float lineTotal = static_cast<float>(m_computedTextForDisplay.lines.size());
    float lineHeight = (m_computedTextForDisplay.textSize.height() / lineTotal) - 10.0F;

    const float IMAGESTARTPOSY = boxStartPosition.y() - ((lineTotal - 1.0F) * (lineHeight + 10.0F)) / m_screenSize.height();
    GLfloat tileCoordIcon[4][2] {
    { -BOXWIDTH + boxStartPosition.x(),  BOXHEIGHT + IMAGESTARTPOSY - 0.06F },     /* Top Left point */
    {  BOXWIDTH + boxStartPosition.x(),  BOXHEIGHT + IMAGESTARTPOSY - 0.06F },     /* Top Right point */
    {  BOXWIDTH + boxStartPosition.x(), -BOXHEIGHT + IMAGESTARTPOSY - 0.06F },     /* Bottom Right point */
    { -BOXWIDTH + boxStartPosition.x(), -BOXHEIGHT + IMAGESTARTPOSY - 0.06F } };   /* Bottom Left point */

    GenerateGLObjectInfo infoGenTextureIcon {
        &m_glObjectIcon,
        &texture.value().get(),
        item->getTextureIndex()};
    GLObjectService::generateGLObject(infoGenTextureIcon, tileCoordIcon, texColorBuf);
}

Point<float> GLTextBox::getBoxStartPosition() const
{
    const float IMAGEHEIGHT = getImageHeight();
    const float COMPUTEDBOXPADDING = getComputedBoxPadding();
    const float BOXHALFHEIGHT = (m_computedTextForDisplay.textSize.height() / m_screenSize.height()) + IMAGEHEIGHT + COMPUTEDBOXPADDING;
    const float STARTPOSY { 0.0F + (BOXHALFHEIGHT / 2.0F) - (COMPUTEDBOXPADDING / 2.0F) - IMAGEHEIGHT };
    return {0.0F, STARTPOSY};
}

float GLTextBox::getBoxHalfHeight() const
{
    return (m_computedTextForDisplay.textSize.height() / m_screenSize.height()) + getImageHeight() + getComputedBoxPadding();
}

float GLTextBox::getComputedBoxPadding() const
{
    return BOXPADDING / m_screenSize.height();
}

float GLTextBox::getImageHeight() const
{
    return (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) ? (60.0F / m_screenSize.height()) : 0.0F;
}

void GLTextBox::drawQuad(const GLObject &glObject, GLuint textureGLIndex)
{
    glBindTexture(GL_TEXTURE_2D, textureGLIndex);
    glBindVertexArray(glObject.vao);
    glBindBuffer(GL_ARRAY_BUFFER, glObject.vboPosition);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, glObject.vboColor);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, glObject.vboTexture);
    glEnableVertexAttribArray(2);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}