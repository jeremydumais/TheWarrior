#include "glTextBox.hpp"
    
void GLTextBox::showMessage(std::shared_ptr<MessageDTO> messageDTO)
{
    constexpr float BOXHALFWIDTH = 200/2;
    constexpr float BOXHALFHEIGHT = 100/2;
    float startPosX { -1.0f + BOXHALFWIDTH };
    float startPosY { 1.0f - BOXHALFHEIGHT };

    GLfloat tileCoord[4][2] = {
    { -BOXHALFWIDTH + startPosX,  BOXHALFHEIGHT + startPosY },     /* Top Left point */
    {  BOXHALFWIDTH + startPosX,  BOXHALFHEIGHT + startPosY },     /* Top Right point */
    {  BOXHALFWIDTH + startPosX, -BOXHALFHEIGHT + startPosY },     /* Bottom Right point */
    { -BOXHALFWIDTH + startPosX, -BOXHALFHEIGHT + startPosY } };   /* Bottom Left point */
    
    glGenBuffers(1, &vboPosition);
    glGenBuffers(1, &vboColor);

    //TODO To continue here (Code below must be relocated)
    /*GenerateGLObjectInfo infoGenTexture {
            nullptr,
            &vao,
            m_glPlayer.getTextureName(),
            m_glPlayer.getTextureIndex(),
            &m_glPlayer.vboPosition,
            &m_glPlayer.vboColor,
            &m_glPlayer.vboTexture };
    generateGLObject(infoGenTexture, tileCoord, m_texColorBuf);*/
}
