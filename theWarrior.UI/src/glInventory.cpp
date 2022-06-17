#include "glInventory.hpp"
#include "point.hpp"
#include <iostream>

const size_t COL_MAX = 7;

GLInventory::GLInventory()
    : m_inventory(nullptr),
      m_inventoryWindowLocation(1.0F, 1.0F),
      m_gameWindowSize(1, 1),
      m_shaderProgram(nullptr),
      m_lastError(""),
      m_inventoryCursorPosition(0),
      m_popupMenuCursorPosition(0),
      m_popupMenuItemCount(0),
      m_inputMode(InventoryInputMode::List),
      m_inventoryMoveSrc(0),
      m_textService(nullptr),
      m_inventorySize({ 1000.0F, 570.0F }),
      m_glTitle({ "Inventory", { 1.0F, 1.0F }, 0.6F }),
      m_glTextDetails({ "Details", { 1.0F, 1.0F }, 0.5F }),
      m_glPopupMenuUse({ "Use", { 1.0F, 1.0F }, 0.6F }),
      m_glPopupMenuMove({ "Drop", { 1.0F, 1.0F }, 0.6F }),
      m_glPopupMenuDrop({ "Move", { 1.0F, 1.0F }, 0.6F }),
      m_slotsGLTexture({ Texture(TextureInfo{ "emptySlot", "item_slot.png", 768, 256, 256, 256 }), 0 }),
      m_inventoryWindowGLTexture({ Texture(TextureInfo { "inventoryWindow", "inventory_window.png", 256, 256, 32, 32 }), 0 })
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
    m_textureService.loadTexture(m_slotsGLTexture);
    m_textureService.loadTexture(m_inventoryWindowGLTexture);
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
    const float SPACING = 10.F;
    const float SLOTSIZE = 80.0F;
    const float ITEMSIZE = 70.0F;

    //Inventory window
    m_inventoryWindowLocation = { (m_gameWindowSize.width() / 2.0F) - (m_inventorySize.width() / 2.0F), 
                                  (m_gameWindowSize.height() / 2.0F) - (m_inventorySize.height() / 2.0F) };
    generateQuad(m_glInventoryWindow, { 0.0F, 0.0F }, m_inventorySize);
    //Detail box
    const Point<float> DETAILSBOXPOSITION = { 16.0F, 80.0F };
    const Size<float> DETAILSBOXSIZE = { 324.0F, 448.0F };
    generateBoxQuad(m_inventoryWindowObjects.begin(),
                    DETAILSBOXPOSITION,
                    DETAILSBOXSIZE,
                    &m_inventoryWindowGLTexture.texture,
                    0);

    //Inventory slots
    for(size_t index = 0; index < m_glSlots.size(); index++) {
        const auto rowAndCol = getRowAndColFromInventoryIndex(index);
        const int textureIndex([this, index]() -> int {
            if (this->m_inputMode != InventoryInputMode::MoveItem && index == this->m_inventoryCursorPosition) {
                return 1;
            }
            if (this->m_inputMode == InventoryInputMode::MoveItem) {
                if (index == this->m_inventoryMoveSrc) {
                    return 1;
                }
                else if(index == this->m_inventoryCursorPosition) {
                    return 2;
                }
            }
            return 0;
        }());
        
        generateQuad(m_glSlots.at(index), 
                     { 360.0F + (rowAndCol.x() * SLOTSIZE) + (rowAndCol.x() * SPACING), 90.0F + (rowAndCol.y() * SLOTSIZE) + (rowAndCol.y() * SPACING) }, 
                     { SLOTSIZE, SLOTSIZE }, 
                     &m_slotsGLTexture.texture, 
                     textureIndex);
    }

    //Inventory objects
    for(const auto &itemMap : m_inventory->getItemsWithIndex()) {
        const auto rowAndCol = getRowAndColFromInventoryIndex(itemMap.first);
        generateQuad(m_glInventoryItems.at(itemMap.first), 
                     { 365.0F + (rowAndCol.x() * SLOTSIZE) + (rowAndCol.x() * SPACING), 100.0F + (rowAndCol.y() * SLOTSIZE) + (rowAndCol.y() * SPACING) }, 
                     { ITEMSIZE, ITEMSIZE }, 
                     &m_itemStore->getTextureContainer().getTextureByName(itemMap.second->getTextureName()).value().get(), 
                     itemMap.second->getTextureIndex());              
    }

    auto titleSize = m_textService->getTextSize(m_glTitle.text, 0.6F);
    m_glTitle.position = { m_inventoryWindowLocation.x() + 15.0F + (m_inventorySize.width() / 2.0F) - (titleSize.width() / 2.0F),
                           m_inventoryWindowLocation.y() + 40.0F };
    m_glTextDetails.position = { m_inventoryWindowLocation.x() + DETAILSBOXPOSITION.x() + 130, 
                                 m_inventoryWindowLocation.y() + DETAILSBOXPOSITION.y() + 36 };
    if (m_inputMode == InventoryInputMode::StatsItemPopup) {
        const Size<float> POPUP_SIZE(300.0F, 260.0F);
        const Point<float> POPUP_LOCATION((m_inventorySize.width() / 2.0F) - (POPUP_SIZE.width() / 2.0F), 
                                          (m_inventorySize.height() / 2.0F) - (POPUP_SIZE.height() / 2.0F));
        const float BLOCKSIZE = 32.0F;
        generateQuad(*m_popupMenuObjects.begin(), 
                     { POPUP_LOCATION.x() + BLOCKSIZE, POPUP_LOCATION.y() + BLOCKSIZE }, 
                     { POPUP_SIZE.width() - (BLOCKSIZE * 2.0F), POPUP_SIZE.height() - (BLOCKSIZE * 2.0F) }, 
                     &m_inventoryWindowGLTexture.texture, 
                     16); 
        generateBoxQuad(m_popupMenuObjects.begin() + 1,
                        { POPUP_LOCATION.x(), POPUP_LOCATION.y() },
                        { POPUP_SIZE.width(), POPUP_SIZE.height() },
                        &m_inventoryWindowGLTexture.texture,
                        8);
        m_glPopupMenuUse.position = { m_inventoryWindowLocation.x() + POPUP_LOCATION.x() + 130, 
                                      m_inventoryWindowLocation.y() + POPUP_LOCATION.y() + 60 };
        m_glPopupMenuMove.position = { m_inventoryWindowLocation.x() + POPUP_LOCATION.x() + 123, 
                                      m_inventoryWindowLocation.y() + POPUP_LOCATION.y() + 130 };
        m_glPopupMenuDrop.position = { m_inventoryWindowLocation.x() + POPUP_LOCATION.x() + 118, 
                                      m_inventoryWindowLocation.y() + POPUP_LOCATION.y() + 200 };
    }
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

void GLInventory::generateBoxQuad(std::array<GLObject, INVENTORY_WINDOW_OBJ_MAX>::iterator begin, 
                                  Point<float> location, 
                                  Size<float> size,
                                  const Texture *texture,
                                  int textureBeginId)
{
    const float BLOCKSIZE = 32.0F;
    //Top left corner
    generateQuad(*begin, 
                 {location.x(), location.y()}, 
                 { BLOCKSIZE, BLOCKSIZE }, 
                 texture, textureBeginId);
                 
    //Top horizontal middle
    generateQuad(*std::next(begin, 1), 
                 {location.x() + BLOCKSIZE, location.y()}, 
                 { size.width() - (BLOCKSIZE * 2.0F), BLOCKSIZE }, 
                 texture, textureBeginId + 1); 

    //Top right corner
    generateQuad(*std::next(begin, 2), 
                 {location.x() + (size.width() - BLOCKSIZE), location.y()}, 
                 { BLOCKSIZE, BLOCKSIZE }, 
                 texture, textureBeginId + 2);

    //Left vertical middle
    generateQuad(*std::next(begin, 3), 
                 {location.x(), location.y() + BLOCKSIZE}, 
                 { BLOCKSIZE, size.height() - (BLOCKSIZE * 2.0F) }, 
                 texture, textureBeginId + 3); 

    //Right vertical middle
    generateQuad(*std::next(begin, 4), 
                 {location.x() + (size.width() - BLOCKSIZE), location.y() + BLOCKSIZE}, 
                 { BLOCKSIZE, size.height() - (BLOCKSIZE * 2.0F) }, 
                 texture, textureBeginId + 4);

    //Bottom left corner
    generateQuad(*std::next(begin, 5), 
                 {location.x(), location.y() + (size.height() - BLOCKSIZE)}, 
                 { BLOCKSIZE, BLOCKSIZE }, 
                 texture, textureBeginId + 5);

    //Bottom horizontal middle
    generateQuad(*std::next(begin, 6), 
                 {location.x() + BLOCKSIZE, location.y() + (size.height() - BLOCKSIZE)}, 
                 { size.width() - (BLOCKSIZE * 2.0F), BLOCKSIZE }, 
                 texture, textureBeginId + 6); 

    //Bottom right corner
    generateQuad(*std::next(begin, 7), 
                 {location.x() + (size.width() - BLOCKSIZE), location.y() + (size.height() - BLOCKSIZE)}, 
                 { BLOCKSIZE, BLOCKSIZE }, 
                 texture, textureBeginId + 7); 
}

Point<float> GLInventory::getRowAndColFromInventoryIndex(size_t index) const
{
    return Point<float>(static_cast<float>(index % COL_MAX),
                        static_cast<float>(index / COL_MAX));
}

void GLInventory::render()
{
    drawQuad(m_glInventoryWindow, 0, 0.8F);
    for(const auto &obj : m_inventoryWindowObjects) {
        drawQuad(obj, m_inventoryWindowGLTexture.glTextureId);
    }
    for (size_t i = 0; i < INVENTORY_MAX; i++) {
        drawQuad(m_glSlots[i], m_slotsGLTexture.glTextureId);
    }
    for(const auto &itemMap : m_inventory->getItemsWithIndex()) {
        drawQuad(m_glInventoryItems.at(itemMap.first), m_texturesGLItemStore->at(itemMap.second->getTextureName()), 0.0F);
    }
    drawText(m_glTitle);
    drawText(m_glTextDetails);
    if (m_inputMode == InventoryInputMode::StatsItemPopup || 
        m_inputMode == InventoryInputMode::ItemPopup || 
        m_inputMode == InventoryInputMode::WeaponOrArmorPopup) {
        for(const auto &obj : m_popupMenuObjects) {
            drawQuad(obj, m_inventoryWindowGLTexture.glTextureId);
        }
    }
    if (m_inputMode == InventoryInputMode::StatsItemPopup) {
        drawText(m_glPopupMenuUse, m_popupMenuCursorPosition == 0 ? GLColor::White : GLColor::Gray);
        drawText(m_glPopupMenuMove, m_popupMenuCursorPosition == 1 ? GLColor::White : GLColor::Gray);
        drawText(m_glPopupMenuDrop, m_popupMenuCursorPosition == 2 ? GLColor::White : GLColor::Gray);
    }
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

void GLInventory::drawText(const GLTextObject &glTextObject, GLColor color)
{   
    m_textService->useShader();
    m_textService->renderText(glTextObject.text, 
                              glTextObject.position.x(),
                              m_gameWindowSize.height() - glTextObject.position.y(),
                              glTextObject.scale,                               // Scale
                              getVec3FromGLColor(color));       // Color
}

void GLInventory::gameWindowSizeChanged(const Size<> &size)
{
    m_gameWindowSize.setSize(static_cast<float>(size.width()),
                             static_cast<float>(size.height()));
    generateGLInventory();
}

void GLInventory::changeMode(InventoryInputMode mode)
{
    m_inputMode = mode;
    generateGLInventory();
}

void GLInventory::processEvents(SDL_Event &e)
{
    switch (m_inputMode)
    {
    case InventoryInputMode::List:
        if(e.type == SDL_KEYDOWN) {
            switch(e.key.keysym.sym) {
                case SDLK_UP:
                    inventoryMoveUpPressed();
                    break;
                case SDLK_DOWN:
                    inventoryMoveDownPressed();
                    break;
                case SDLK_LEFT:
                    inventoryMoveLeftPressed();
                    break;
                case SDLK_RIGHT:
                    inventoryMoveRightPressed();
                    break;
            };
        }
        else if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_RETURN) {
            inventoryActionButtonPressed();
        }
        break;
    case InventoryInputMode::MoveItem:
        if(e.type == SDL_KEYDOWN) {
            switch(e.key.keysym.sym) {
                case SDLK_UP:
                    inventoryMoveUpPressed();
                    break;
                case SDLK_DOWN:
                    inventoryMoveDownPressed();
                    break;
                case SDLK_LEFT:
                    inventoryMoveLeftPressed();
                    break;
                case SDLK_RIGHT:
                    inventoryMoveRightPressed();
                    break;
            };
        }
        else if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_RETURN) {
            MoveActionButtonPressed();
        }
        break;
    case InventoryInputMode::StatsItemPopup:
        if(e.type == SDL_KEYDOWN) {
            switch(e.key.keysym.sym) {
                case SDLK_UP:
                    popupMenuMoveUpPressed();
                    break;
                case SDLK_DOWN:
                    popupMenuMoveDownPressed();
                    break;
            }
        }
        else if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_RETURN) {
            statsItemActionButtonPressed();
        }
        else if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE) {
            m_inputMode = InventoryInputMode::List;
            generateGLInventory();
        }
        break;
    default:
        break;
    }   
}

void GLInventory::inventoryMoveUpPressed()
{
    if (m_inventoryCursorPosition >= COL_MAX) {
        m_inventoryCursorPosition -= COL_MAX;
        generateGLInventory();
    }
}

void GLInventory::inventoryMoveDownPressed()
{    
    if (m_inventoryCursorPosition + COL_MAX < INVENTORY_MAX) {
        m_inventoryCursorPosition += COL_MAX;
        generateGLInventory();
    }
}

void GLInventory::inventoryMoveLeftPressed()
{
    if (m_inventoryCursorPosition > 0) {
        m_inventoryCursorPosition--;
        generateGLInventory();
    }
}

void GLInventory::inventoryMoveRightPressed()
{
    if (m_inventoryCursorPosition + 1 != INVENTORY_MAX) {
        m_inventoryCursorPosition++;
        generateGLInventory();
    }
}

void GLInventory::inventoryActionButtonPressed()
{
    switch (m_inputMode)
    {
    case InventoryInputMode::List:
        {
            //Check if the cell contains an Item
            auto item = m_inventory->getItem(m_inventoryCursorPosition);
            if (item) {
                if (item->getType() == ItemType::StatsItem) {
                    m_popupMenuCursorPosition = 0;
                    m_popupMenuItemCount = 3;
                    changeMode(InventoryInputMode::StatsItemPopup);
                }
            }
        }
        break;
    default:
        break;
    }
}

void GLInventory::popupMenuMoveUpPressed()
{
    if (m_popupMenuCursorPosition > 0) {
        m_popupMenuCursorPosition--;
        generateGLInventory();
    }
}

void GLInventory::popupMenuMoveDownPressed()
{
    if (m_popupMenuCursorPosition + 1 < m_popupMenuItemCount) {
        m_popupMenuCursorPosition++;
        generateGLInventory();
    }
}

void GLInventory::statsItemActionButtonPressed()
{
    if (m_popupMenuCursorPosition == 2) {
        m_inventoryMoveSrc = m_inventoryCursorPosition;
        changeMode(InventoryInputMode::MoveItem);
    }
}

void GLInventory::MoveActionButtonPressed()
{
    m_inventory->moveItem(m_inventoryMoveSrc, m_inventoryCursorPosition);
    changeMode(InventoryInputMode::List);

}