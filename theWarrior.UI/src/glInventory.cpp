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
      m_inputMode(InventoryInputMode::List),
      m_inventoryMoveSrc(0),
      m_glFormService(std::make_shared<GLFormService>()),
      m_textService(nullptr),
      m_inventorySize({ 1000.0F, 570.0F }),
      m_glTitle({ "Inventory", { 1.0F, 1.0F }, 0.6F }),
      m_glTextDetails({ "Details", { 1.0F, 1.0F }, 0.5F }),
      m_glSlots(std::vector<GLObject>(INVENTORY_MAX)),
      m_slotsGLTexture({ Texture(TextureInfo{ "emptySlot", "item_slot.png", 768, 256, 256, 256 }), 0 }),
      m_glInventoryItems(std::vector<GLObject>(INVENTORY_MAX)),
      m_inventoryWindowObjects(std::vector<GLObject>(INVENTORY_WINDOW_OBJ_MAX)),
      m_inventoryWindowGLTexture({ Texture(TextureInfo { "inventoryWindow", "inventory_window.png", 256, 256, 32, 32 }), 0 })
{
    m_choicePopup.m_choiceClicked.connect(boost::bind(&GLInventory::itemActionPopupClicked, this, boost::placeholders::_1));
    m_choicePopup.m_cancelClicked.connect(boost::bind(&GLInventory::itemActionPopupCanceled, this));
}

bool GLInventory::initShader(const std::string &vertexShaderFileName,
                             const std::string &fragmentShaderFileName)
{
    m_shaderProgram = std::make_shared<GLShaderProgram>(vertexShaderFileName,
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
    m_glFormService->initialize(m_shaderProgram, textService);
    m_choicePopup.initialize(resourcePath, m_glFormService, textService);
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
    m_glFormService->generateQuad(m_glInventoryWindow, m_inventoryWindowLocation, m_inventorySize);
    //Detail box
    const Point<float> DETAILSBOXPOSITION = { m_inventoryWindowLocation.x() + 16.0F, 
                                              m_inventoryWindowLocation.y() + 80.0F };
    const Size<float> DETAILSBOXSIZE = { 324.0F, 448.0F };
    m_glFormService->generateBoxQuad(m_inventoryWindowObjects.begin(),
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
        
        m_glFormService->generateQuad(m_glSlots.at(index), 
                                    { m_inventoryWindowLocation.x() + 360.0F + (rowAndCol.x() * SLOTSIZE) + (rowAndCol.x() * SPACING), 
                                    m_inventoryWindowLocation.y() + 90.0F + (rowAndCol.y() * SLOTSIZE) + (rowAndCol.y() * SPACING) }, 
                                    { SLOTSIZE, SLOTSIZE }, 
                                    &m_slotsGLTexture.texture, 
                                    textureIndex);
    }

    //Inventory objects
    for(const auto &itemMap : m_inventory->getItemsWithIndex()) {
        const auto rowAndCol = getRowAndColFromInventoryIndex(itemMap.first);
        m_glFormService->generateQuad(m_glInventoryItems.at(itemMap.first), 
                                    { m_inventoryWindowLocation.x() + 365.0F + (rowAndCol.x() * SLOTSIZE) + (rowAndCol.x() * SPACING), 
                                    m_inventoryWindowLocation.y() + 100.0F + (rowAndCol.y() * SLOTSIZE) + (rowAndCol.y() * SPACING) }, 
                                    { ITEMSIZE, ITEMSIZE }, 
                                    &m_itemStore->getTextureContainer().getTextureByName(itemMap.second->getTextureName()).value().get(), 
                                    itemMap.second->getTextureIndex());              
    }

    auto titleSize = m_textService->getTextSize(m_glTitle.text, 0.6F);
    m_glTitle.position = { m_inventoryWindowLocation.x() + 15.0F + (m_inventorySize.width() / 2.0F) - (titleSize.width() / 2.0F),
                           m_inventoryWindowLocation.y() + 40.0F };
    m_glTextDetails.position = { DETAILSBOXPOSITION.x() + 130, 
                                 DETAILSBOXPOSITION.y() + 36 };
    if (m_inputMode == InventoryInputMode::StatsItemPopup ||
        m_inputMode == InventoryInputMode::DropItemPopup) {
        m_choicePopup.generateGLInventory();
    }
}

Point<float> GLInventory::getRowAndColFromInventoryIndex(size_t index) const
{
    return Point<float>(static_cast<float>(index % COL_MAX),
                        static_cast<float>(index / COL_MAX));
}

void GLInventory::render()
{
    m_glFormService->drawQuad(m_glInventoryWindow, 0, 0.8F);
    for(const auto &obj : m_inventoryWindowObjects) {
        m_glFormService->drawQuad(obj, m_inventoryWindowGLTexture.glTextureId);
    }
    for (size_t i = 0; i < INVENTORY_MAX; i++) {
        m_glFormService->drawQuad(m_glSlots[i], m_slotsGLTexture.glTextureId);
    }
    for(const auto &itemMap : m_inventory->getItemsWithIndex()) {
        m_glFormService->drawQuad(m_glInventoryItems.at(itemMap.first), m_texturesGLItemStore->at(itemMap.second->getTextureName()), 0.0F);
    }
    m_glFormService->drawText(m_glTitle);
    m_glFormService->drawText(m_glTextDetails);
    if (m_inputMode == InventoryInputMode::StatsItemPopup ||
        m_inputMode == InventoryInputMode::DropItemPopup) {
        m_choicePopup.render();
    }
}

void GLInventory::gameWindowSizeChanged(const Size<> &size)
{
    m_gameWindowSize.setSize(static_cast<float>(size.width()),
                             static_cast<float>(size.height()));
    m_inventoryWindowLocation = { (m_gameWindowSize.width() / 2.0F) - (m_inventorySize.width() / 2.0F), 
                                  (m_gameWindowSize.height() / 2.0F) - (m_inventorySize.height() / 2.0F) };
    m_glFormService->gameWindowSizeChanged(size);
    const Point<float> inventoryWindowCenter(m_inventoryWindowLocation.x() + (m_inventorySize.width() / 2.0F),
                                             m_inventoryWindowLocation.y() + (m_inventorySize.height() / 2.0F));
    m_choicePopup.gameInventoryLocationChanged(inventoryWindowCenter);
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
            completeMoveActionButtonPressed();
        }
        else if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE) {
            changeMode(InventoryInputMode::List);
        }
        break;
    case InventoryInputMode::StatsItemPopup:
        m_choicePopup.processEvents(e);
        break;
    case InventoryInputMode::DropItemPopup:
        m_choicePopup.processEvents(e);
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
                    m_choicePopup.preparePopup({ "Use", "Move", "Drop" });
                    changeMode(InventoryInputMode::StatsItemPopup);
                }
            }
        }
        break;
    default:
        break;
    }
}

void GLInventory::itemActionPopupClicked(size_t choice)
{
    switch (m_inputMode)
    {
    case InventoryInputMode::StatsItemPopup:
        if (choice == 1) {
            m_inventoryMoveSrc = m_inventoryCursorPosition;
            changeMode(InventoryInputMode::MoveItem);
        }
        else if (choice == 2) {
            m_choicePopup.preparePopup({ "No", "Yes" });
            changeMode(InventoryInputMode::DropItemPopup);
        }
        break;
    case InventoryInputMode::DropItemPopup :
        if (choice == 0) {
            changeMode(InventoryInputMode::List);
        }
        else if (choice == 1) {
            m_inventory->dropItem(m_inventoryCursorPosition);
            changeMode(InventoryInputMode::List);
        }
        break;
    default:
        break;
    }
    
}
void GLInventory::itemActionPopupCanceled()
{
    changeMode(InventoryInputMode::List);
}

void GLInventory::completeMoveActionButtonPressed()
{
    m_inventory->moveItem(m_inventoryMoveSrc, m_inventoryCursorPosition);
    changeMode(InventoryInputMode::List);
}