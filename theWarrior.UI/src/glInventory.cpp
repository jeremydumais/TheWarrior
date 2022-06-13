#include "glInventory.hpp"
#include "point.hpp"
#include <iostream>

GLInventory::GLInventory()
    : m_inventory(nullptr),
      m_inventoryWindowLocation(1.0F, 1.0F),
      m_gameWindowSize(1, 1),
      m_shaderProgram(nullptr),
      m_lastError(""),
      m_textService(nullptr),
      m_inventorySize({ 1000.0F, 570.0F }),
      m_glTitle({ "Inventory", { 1.0F, 1.0F }, 0.6F }),
      m_slotsTexture(TextureInfo{ "emptySlot", "item_slot.png", 256, 256, 256, 256 })
{
}

bool GLInventory::initShader(const std::string &vertexShaderFileName,
                             const std::string &fragmentShaderFileName)
{
    m_shaderProgram = std::make_unique<GLShaderProgram>(vertexShaderFileName,
                                                        fragmentShaderFileName);
    if (!m_shaderProgram->compileShaders()) {
        m_lastError = m_shaderProgram->getLastError();
        return false;
    }
    if (!m_shaderProgram->linkShaders({ "vertex" })) {
        m_lastError = m_shaderProgram->getLastError();
        return false;
    }
    return true;
}

void GLInventory::initialize(const std::string &resourcePath,
                             std::shared_ptr<GLTextService> textService,
                             std::shared_ptr<ItemStore> itemStore,
                             const std::map<std::string, unsigned int> *texturesGLItemStore)
{
    m_textService = textService;
    m_textureService.setResourcesPath(resourcePath);
    m_textureService.loadTexture(m_slotsTexture, m_emptySlotGLTextureId);
    m_itemStore = itemStore;
    m_texturesGLItemStore = texturesGLItemStore;
}

const std::string& GLInventory::getLastError() const
{
    return m_lastError;
}

void GLInventory::setInventory(std::shared_ptr<Inventory> inventory)
{
    m_inventory = inventory;
}

void GLInventory::generateGLInventory()
{
    m_inventoryWindowLocation = { (m_gameWindowSize.width() / 2.0F) - (m_inventorySize.width() / 2.0F), 
                                  (m_gameWindowSize.height() / 2.0F) - (m_inventorySize.height() / 2.0F) };

    generateQuad(m_glInventoryWindow, { 0.0F, 0.0F }, m_inventorySize);

    const size_t COLS = 7;
    const size_t ROWS = 5;
    const float SPACING = 90.F;
    const float SLOTSIZE = 80.0F;
    float x = 360.0F;
    float y = 90.0F;
    for (size_t i = 0; i < ROWS; i++) {
        for (size_t j = 0; j < COLS; j++) {
            generateQuad(m_slots.at((i * COLS) + j), 
                         { x, y }, 
                         { SLOTSIZE, SLOTSIZE}, 
                         &m_slotsTexture, 
                         static_cast<int>(m_emptySlotGLTextureId));
            x += SPACING;
        }   
        y += SPACING;
        x = 360.0F;
    }
    auto tex = m_itemStore->getTextureContainer().getTextureByName("ItemsTile").value();
    generateQuad(itemTest, { 370.0F, 100.0F }, { 60.0F, 60.0F }, &tex.get(), 336);
    
    auto titleSize = m_textService->getTextSize(m_glTitle.text, 0.6F);
    m_glTitle.position.setX(m_inventoryWindowLocation.x() + 40.0F + (m_inventorySize.width() / 2.0F) - (titleSize.width() / 2.0F));
    m_glTitle.position.setY(m_inventoryWindowLocation.y() + 40.0F);
}

void GLInventory::generateQuad(GLObject &object, Point<float> location, Size<float> size, const Texture *texture, int textureId)
{
    GLfloat texColorBuf[4][3];
    //Limits: -1.0F to 1.0F (top left corner: -1.0F, 1.0F - bottom right corner: 1.0F, -1.0F)
    auto invLocationX = m_inventoryWindowLocation.x() + location.x();
    auto invLocationY = m_inventoryWindowLocation.y() + location.y();
    auto gameWinW = m_gameWindowSize.width();
    auto gameWinH = m_gameWindowSize.height();
    const Point<float> normalizedLocation(((invLocationX - (gameWinW / 2.0F)) / gameWinW) * 2.0F,
                                          (((gameWinH - invLocationY) - (gameWinH / 2.0F)) / gameWinH) * 2.0F);

    const float BOXWIDTH = size.width() / gameWinW;
    const float BOXHEIGHT = size.height() / gameWinH;

    GLfloat tileCoord[4][2] = {
    { normalizedLocation.x(), normalizedLocation.y() }, /* Top Left point */
    { normalizedLocation.x() + (BOXWIDTH * 2.0F), normalizedLocation.y()  }, /* Top Right point */
    { normalizedLocation.x() + (BOXWIDTH * 2.0F), normalizedLocation.y() - (BOXHEIGHT * 2.0F)}, /* Bottom Right point */
    { normalizedLocation.x(), normalizedLocation.y() - (BOXHEIGHT * 2.0F)} }; /* Bottom Left point */
    
    if (!texture) {
        GenerateGLObjectInfo infoGenTexture {
            &object,
            nullptr,
            -1};
        GLObjectService::generateGLObject(infoGenTexture, tileCoord, texColorBuf);
    }
    else {
        GenerateGLObjectInfo infoGenTexture {
            &object,
            texture,
            textureId};
        GLObjectService::generateGLObject(infoGenTexture, tileCoord, texColorBuf);
    }
}

void GLInventory::render()
{
    drawQuad(m_glInventoryWindow, 0, 0.8F);
    for (size_t i = 0; i < INVENTORY_MAX; i++) {
        drawQuad(m_slots[i], m_emptySlotGLTextureId);
    }
    //TODO Loop through items in inventory
    drawQuad(itemTest, m_texturesGLItemStore->at("ItemsTile"), 0.0F);
    drawText(m_glTitle);
}
void GLInventory::drawQuad(const GLObject &glObject, GLuint textureGLIndex, float transparency)
{
    m_shaderProgram->use();
    GLint uniformTransparency = glGetUniformLocation(m_shaderProgram->getShaderProgramID(), "transparency");
    glUniform1f(uniformTransparency, transparency);
    glActiveTexture(GL_TEXTURE0);
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

void GLInventory::drawText(const GLTextObject &glTextObject)
{   
    m_textService->useShader();
    m_textService->renderText(glTextObject.text, 
                              glTextObject.position.x(),
                              m_gameWindowSize.height() - glTextObject.position.y(),
                              glTextObject.scale,                               // Scale
                              glm::vec3(1.0f, 1.0f, 1.0f));       // Color
}

void GLInventory::gameWindowSizeChanged(const Size<> &size)
{
    m_gameWindowSize.setSize(static_cast<float>(size.width()),
                             static_cast<float>(size.height()));
    generateGLInventory();
}