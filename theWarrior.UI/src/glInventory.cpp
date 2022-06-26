#include "glInventory.hpp"
#include "armorItem.hpp"
#include "point.hpp"
#include "statsItem.hpp"
#include "weaponItem.hpp"
#include <fmt/format.h>
#include <iostream>

const size_t COL_MAX = 7;
const float SPACING = 10.F;
const float SLOTSIZE = 80.0F;
const float ITEMSIZE = 70.0F;
const Size<float> DETAILSBOXSIZE = { 324.0F, 448.0F };

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
      m_glDetailsTextTitle({ "Details", { 1.0F, 1.0F }, 0.5F }),
      m_glDetailsTextObjects(std::vector<GLTextObject>()),
      m_detailsBoxPosition({ 1.0F, 1.0F }),
      m_glSlots(std::vector<GLObject>(INVENTORY_MAX)),
      m_slotsGLTexture({ Texture(TextureInfo{ "emptySlot", "item_slot.png", 768, 256, 256, 256 }), 0 }),
      m_glInventoryItems(std::vector<GLObject>(INVENTORY_MAX)),
      m_inventoryWindowObjects(std::vector<GLObject>(INVENTORY_WINDOW_OBJ_MAX)),
      m_inventoryWindowGLTexture({ Texture(TextureInfo { "inventoryWindow", "window.png", 256, 256, 32, 32 }), 0 })
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
                             std::shared_ptr<GLPlayer> glPlayer,
                             std::shared_ptr<GLTextService> textService,
                             std::shared_ptr<ItemStore> itemStore,
                             const std::map<std::string, unsigned int> *texturesGLItemStore)
{
    m_glPlayer = glPlayer;
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
    //Inventory window
    m_glFormService->generateQuad(m_glInventoryWindow, m_inventoryWindowLocation, m_inventorySize);
    m_glFormService->generateBoxQuad(m_inventoryWindowObjects.begin(),
                                    m_inventoryWindowLocation,
                                    m_inventorySize,
                                    &m_inventoryWindowGLTexture.texture,
                                    17);
    //TitleBox
    auto titleSize = m_textService->getTextSize(m_glTitle.text, 0.6F);
    m_glTitle.position = { m_inventoryWindowLocation.x() + 15.0F + (m_inventorySize.width() / 2.0F) - (titleSize.width() / 2.0F),
                           m_inventoryWindowLocation.y() + 40.0F };
    m_glFormService->generateBoxQuad(m_inventoryWindowObjects.begin()+8,
                                    {m_glTitle.position.x() - 35.0F, m_glTitle.position.y() - 30.0F},
                                    {200.0F, 40.0F},
                                    &m_inventoryWindowGLTexture.texture,
                                    17);
    //Detail box
    m_glFormService->generateBoxQuad(m_inventoryWindowObjects.begin()+16,
                                    m_detailsBoxPosition,
                                    DETAILSBOXSIZE,
                                    &m_inventoryWindowGLTexture.texture,
                                    0);

    generateSlots();

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

    generateDetailsInfo();         
    if (m_inputMode == InventoryInputMode::StatsItemPopup ||
        m_inputMode == InventoryInputMode::ItemPopup ||
        m_inputMode == InventoryInputMode::WeaponOrArmorPopup ||
        m_inputMode == InventoryInputMode::DropItemPopup) {
        m_choicePopup.generateGLInventory();
    }
}

void GLInventory::generateSlots()
{
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
}

void GLInventory::generateDetailsInfo()
{
    auto detailSize = m_textService->getTextSize(m_glDetailsTextTitle.text, 0.5F);
    m_glDetailsTextTitle.position = { m_detailsBoxPosition.x() + (DETAILSBOXSIZE.width() / 2.0F) - (detailSize.width() / 2.0F), 
                                      m_detailsBoxPosition.y() + 36 };
    m_glDetailsTextObjects.clear();
    auto item = m_inventory->getItem(m_inventoryCursorPosition);
    if (item != nullptr) {
        generateDetailLabelXCentered(item->getName(), 80.0F, 0.4F);
        int index = 0;
        if (!item->getOptionalDescription().empty()) {
            auto result = m_textService->prepareTextForDisplay(DETAILSBOXSIZE, item->getOptionalDescription(), 0.4F);
            for(const auto &str : result.lines) {
                generateDetailLabelXCentered(str, 120.0F + (static_cast<float>(index) * 25.0F), 0.4F);
                index++;                                                                   
            }
        }
        float nextItemAfterDescY = index > 0 ? 120.0F + (static_cast<float>(index) * 25.0F) + 25.0F : 120.0F;
        switch (item->getType())
        {
        case ItemType::Weapon:
            generateWeaponDetails(item, nextItemAfterDescY);
            break;    
        case ItemType::StatsItem:
            generateStatsItemDetails(item, nextItemAfterDescY);
            break;
        default:
            break;
        } 
    }
}

void GLInventory::generateWeaponDetails(std::shared_ptr<const Item> item, float yPosition)
{
    const auto *weapon = dynamic_cast<const WeaponItem*>(item.get());
    if (weapon) {
        std::string attackLabel = "Attack: ";
        auto equipment = m_glPlayer->getEquipment();
        auto currentAttackGain = [weapon, equipment]() {
            if (weapon->getSlotInBodyPart() == WeaponBodyPart::MainHand) {
                auto currentWeapon = equipment.getMainHand();
                if (currentWeapon.has_value()) {
                    return currentWeapon->getAttackGain();
                }
            }
            return 0.0F;
        }();

        auto gainDifference = weapon->getAttackGain() - currentAttackGain;
        std::string attackGainSymbol = gainDifference >=0 ? "+": "";
        std::string attackValueStr = fmt::format("{0}{1}", attackGainSymbol, gainDifference);
        GLColor valueColor = [gainDifference]() {
            if (gainDifference > 0) return GLColor::Green;
            if (gainDifference < 0) return GLColor::Red;
            return GLColor::White;
        }();
        generateTwoColumnsLabels("Attack: ", attackValueStr, yPosition, 0.4F, 
                                 GLColor::White, valueColor);
        generateTwoColumnsLabels("Slot in: ", WeaponItem::getBodyPartAsString(weapon->getSlotInBodyPart()), yPosition + 25.0F, 0.4F);
    }
}

void GLInventory::generateStatsItemDetails(std::shared_ptr<const Item> item, float yPosition)
{
    const auto *statsItem = dynamic_cast<const StatsItem *>(item.get());
    if (statsItem && statsItem->getLimitOfOneApplied()) {
        generateDetailLabelXCentered("Limit of one applied.", yPosition, 0.4F, GLColor::LightGray);
    }
    if (statsItem && statsItem->getDurationInSecs() > 0) {
        generateDetailLabelXCentered(fmt::format("Last {0} seconds.", statsItem->getDurationInSecs()), 
                            yPosition + 25.0F, 
                            0.4F, GLColor::LightGray);
    }
}
void GLInventory::generateDetailLabel(const std::string &text,
                                      float xOffsetFromCenter,
                                      float yPosition,
                                      float scale,
                                      GLColor color)
{
    float detailBoxXCenter = m_detailsBoxPosition.x() + (DETAILSBOXSIZE.width() / 2.0F);
    auto labelSize = m_textService->getTextSize(text, scale);
    m_glDetailsTextObjects.emplace_back(GLTextObject({ text, 
                                                       { detailBoxXCenter + xOffsetFromCenter - (labelSize.width() / 2.0F), 
                                                         m_detailsBoxPosition.y() + yPosition }, 
                                                       scale,
                                                       color }));
}

void GLInventory::generateDetailLabelXCentered(const std::string &text,
                                               float yPosition,
                                               float scale, 
                                               GLColor color)
{
    generateDetailLabel(text, 0.0F, yPosition, scale, color);
}

void GLInventory::generateTwoColumnsLabels(const std::string &label, 
                                           const std::string &value,
                                           float yPosition,
                                           float scale,
                                           GLColor colorLabel,
                                           GLColor colorValue)
{
    auto labelStrSize = m_textService->getTextSize(label, scale);
    auto labelAndValueSize = m_textService->getTextSize(fmt::format("{0}{1}", label, value), scale); 

    generateDetailLabel(label, -(labelAndValueSize.width() / 2.0F) + (labelStrSize.width() / 2.0F), yPosition, scale, colorLabel);
    generateDetailLabel(value, 
                        (labelAndValueSize.width() / 2.0F) - ((labelAndValueSize.width() / 2.0F) - (labelStrSize.width() / 2.0F)), 
                        yPosition, scale, colorValue);
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
    m_glFormService->drawText(m_glDetailsTextTitle);
    for(const auto &text : m_glDetailsTextObjects) {
        m_glFormService->drawText(text);
    }
    if (m_inputMode == InventoryInputMode::StatsItemPopup ||
        m_inputMode == InventoryInputMode::ItemPopup ||
        m_inputMode == InventoryInputMode::WeaponOrArmorPopup ||
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
    m_detailsBoxPosition = { m_inventoryWindowLocation.x() + 16.0F, 
                             m_inventoryWindowLocation.y() + 80.0F };
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
    case InventoryInputMode::ItemPopup:
        m_choicePopup.processEvents(e);
        break;
    case InventoryInputMode::WeaponOrArmorPopup:
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
                else if (item->getType() == ItemType::Armor || item->getType() == ItemType::Weapon) {
                    m_choicePopup.preparePopup({ "Equip", "Move", "Drop" });
                    changeMode(InventoryInputMode::WeaponOrArmorPopup);
                }
                else if (item->getType() == ItemType::Item) {
                    m_choicePopup.preparePopup({ "Move", "Drop" });
                    changeMode(InventoryInputMode::ItemPopup);
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
            prepareMoveItemMode();
        }
        else if (choice == 2) {
            prepareDropItemPopup();
        }
        break;
    case InventoryInputMode::ItemPopup:
        if (choice == 0) {
            prepareMoveItemMode();
        }
        else if (choice == 1) {
            prepareDropItemPopup();
        }
        break;
    case InventoryInputMode::WeaponOrArmorPopup:
        if (choice == 1) {
            prepareMoveItemMode();
        }
        else if (choice == 2) {
            prepareDropItemPopup();
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

void GLInventory::prepareMoveItemMode()
{
    m_inventoryMoveSrc = m_inventoryCursorPosition;
    changeMode(InventoryInputMode::MoveItem);
}

void GLInventory::prepareDropItemPopup()
{
    m_choicePopup.preparePopup({ "No", "Yes" });
    changeMode(InventoryInputMode::DropItemPopup);
}
