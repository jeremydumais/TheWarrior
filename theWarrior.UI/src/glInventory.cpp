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
    : GLPopupWindow({ 1000.0F, 570.0F }),
      m_inventory(nullptr),
      m_inventoryCursorPosition(0),
      m_inputMode(InventoryInputMode::List),
      m_inventoryMoveSrc(0),
      m_detailsBoxPosition({ 1.0F, 1.0F }),
      m_slotsGLTexture({ Texture(TextureInfo{ "emptySlot", "item_slot.png", 768, 256, 256, 256 }), 0 }),
      m_inventoryIconsGLTexture({ Texture(TextureInfo{ "inventory", "inventory.png", 96, 32, 32, 32 }), 0 }),
      m_joystick(nullptr)
{
    m_choicePopup.m_choiceClicked.connect(boost::bind(&GLInventory::itemActionPopupClicked, this, boost::placeholders::_1));
    m_choicePopup.m_cancelClicked.connect(boost::bind(&GLInventory::itemActionPopupCanceled, this));
}

void GLInventory::initialize(const std::string &resourcePath,
                             std::shared_ptr<GLPlayer> glPlayer,
                             std::shared_ptr<GLTextService> textService,
                             std::shared_ptr<ItemStore> itemStore,
                             const std::map<std::string, unsigned int> *texturesGLItemStore,
                             SDL_Joystick *joystick)
{
    GLPopupWindow::initialize("Inventory", resourcePath, textService);
    m_glPlayer = glPlayer;
    m_textureService.loadTexture(m_slotsGLTexture);
    m_textureService.loadTexture(m_inventoryIconsGLTexture);
    m_itemStore = itemStore;
    m_texturesGLItemStore = texturesGLItemStore;
    m_joystick = joystick;
    m_choicePopup.initialize(resourcePath, m_glFormService, textService, joystick);
}

void GLInventory::setInventory(std::shared_ptr<Inventory> inventory)
{
    m_inventory = inventory;
}

void GLInventory::generateGLInventory()
{
    GLPopupWindow::generateGLElements();
    //Detail box
    generateBoxQuad(m_glObjects,
                    m_detailsBoxPosition,
                    DETAILSBOXSIZE,
                    &m_windowGLTexture.texture,
                    0,
                    m_windowGLTexture.glTextureId);

    generateSlots();

    //Inventory objects
    for(const auto &itemMap : m_inventory->getItemsWithIndex()) {
        const auto rowAndCol = getRowAndColFromInventoryIndex(itemMap.first);
        generateQuad(m_glObjects, 
                     { 365.0F + (rowAndCol.x() * SLOTSIZE) + (rowAndCol.x() * SPACING), 
                         100.0F + (rowAndCol.y() * SLOTSIZE) + (rowAndCol.y() * SPACING) }, 
                     { ITEMSIZE, ITEMSIZE }, 
                     &m_itemStore->getTextureContainer().getTextureByName(itemMap.second->getTextureName()).value().get(), 
                     itemMap.second->getTextureIndex(),
                     m_texturesGLItemStore->at(itemMap.second->getTextureName()));              
    }

    const auto stats = m_glPlayer->getStats();
    generateQuad(m_glObjects, {660.0F, 15.0F}, {32.0F, 32.0F}, &m_inventoryIconsGLTexture.texture, 0, m_inventoryIconsGLTexture.glTextureId);
    addTextObject({fmt::format("{0}/{1}", stats.health, stats.maxHealth), {700.0F, 36.0F}, 0.3F});
    generateQuad(m_glObjects, {800.0F, 15.0F}, {32.0F, 32.0F}, &m_inventoryIconsGLTexture.texture, 1, m_inventoryIconsGLTexture.glTextureId);
    addTextObject({fmt::format("{0}", stats.attack), {840.0F, 36.0F}, 0.3F});
    generateQuad(m_glObjects, {900.0F, 15.0F}, {32.0F, 32.0F}, &m_inventoryIconsGLTexture.texture, 2, m_inventoryIconsGLTexture.glTextureId);
    addTextObject({fmt::format("{0}", stats.defense), {940.0F, 36.0F}, 0.3F});
    generateDetailsInfo();         
    if (m_inputMode == InventoryInputMode::StatsItemPopup ||
        m_inputMode == InventoryInputMode::ItemPopup ||
        m_inputMode == InventoryInputMode::WeaponOrArmorPopup ||
        m_inputMode == InventoryInputMode::DropItemPopup) {
        m_choicePopup.generateGLElements();
    }
}

void GLInventory::generateSlots()
{
    for(size_t index = 0; index < INVENTORY_MAX; index++) {
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
        
        generateQuad(m_glObjects, 
                     { 360.0F + (rowAndCol.x() * SLOTSIZE) + (rowAndCol.x() * SPACING), 
                       90.0F + (rowAndCol.y() * SLOTSIZE) + (rowAndCol.y() * SPACING) }, 
                     { SLOTSIZE, SLOTSIZE }, 
                     &m_slotsGLTexture.texture, 
                     textureIndex,
                     m_slotsGLTexture.glTextureId);
    }
}

void GLInventory::generateDetailsInfo()
{
    addXCenteredTextObject({"Details", {0.0F, m_detailsBoxPosition.y() + 36}, 0.5F}, m_detailsBoxPosition.x(), DETAILSBOXSIZE.width());
    auto item = m_inventory->getItem(m_inventoryCursorPosition);
    if (item != nullptr) {
        //Display icon
        auto iconTexture = &m_itemStore->getTextureContainer().getTextureByName(item->getTextureName()).value().get(); 
        m_glDetailsIconTextureId = m_texturesGLItemStore->at(item->getTextureName());
        Point<float> iconPosition( m_detailsBoxPosition.x() + (DETAILSBOXSIZE.width() / 2.0F) - (SLOTSIZE / 2.0F), 
                                   m_detailsBoxPosition.y() + 60.0F);
        generateQuad(m_glObjects, 
                     iconPosition, 
                     {SLOTSIZE, SLOTSIZE}, 
                     iconTexture, 
                     item->getTextureIndex(), 
                     m_texturesGLItemStore->at(item->getTextureName()));
        generateDetailLabelXCentered(item->getName(), 160.0F, 0.4F);
        int index = 0;
        if (!item->getOptionalDescription().empty()) {
            auto result = m_textService->prepareTextForDisplay(DETAILSBOXSIZE, item->getOptionalDescription(), 0.4F);
            for(const auto &str : result.lines) {
                generateDetailLabelXCentered(str, 200.0F + (static_cast<float>(index) * 25.0F), 0.4F);
                index++;                                                                   
            }
        }
        float nextItemAfterDescY = index > 0 ? 200.0F + (static_cast<float>(index) * 25.0F) + 25.0F : 200.0F;
        switch (item->getType())
        {
        case ItemType::Armor:
            generateArmorDetails(item, nextItemAfterDescY);
            break;  
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
    else {
        m_glDetailsIconTextureId = 0;
    }
}

void GLInventory::generateWeaponDetails(std::shared_ptr<const Item> item, float yPosition)
{
    const auto *weapon = dynamic_cast<const WeaponItem*>(item.get());
    if (weapon) {
        const auto &equipment = m_glPlayer->getEquipment();
        bool showDefenseLossSection = false;
        float defenseLoss = 0.0F;
        auto currentAttackGain = [weapon, equipment]() {
            if (weapon->getSlotInBodyPart() == WeaponBodyPart::MainHand) {
                auto currentWeapon = equipment.getMainHand();
                if (currentWeapon.has_value()) {
                    return currentWeapon->getAttackGain();
                }
            }
            else if (weapon->getSlotInBodyPart() == WeaponBodyPart::SecondaryHand &&
                     equipment.getSecondaryHandType() == SecondaryHandType::Weapon) {
                auto currentWeapon = equipment.getSecondaryHand();
                return boost::get<WeaponItem>(currentWeapon.get()).getAttackGain();
            }
            return 0.0F;
        }();
        if (weapon->getSlotInBodyPart() == WeaponBodyPart::SecondaryHand && 
            equipment.getSecondaryHandType() == SecondaryHandType::Armor) {
            showDefenseLossSection = true;
            auto currentArmor = equipment.getSecondaryHand();
            defenseLoss = boost::get<ArmorItem>(currentArmor.get()).getDefenseGain() * -1.0F;
        }

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
        float nextSectionY = 25.0F;
        if(showDefenseLossSection) {
            generateTwoColumnsLabels("Defense: ", fmt::format("{0}", defenseLoss), yPosition + nextSectionY, 0.4F, 
                                     GLColor::White, GLColor::Red);   
            nextSectionY += 25.0F; 
        }
        generateTwoColumnsLabels("Slot in: ", WeaponItem::getBodyPartAsString(weapon->getSlotInBodyPart()), yPosition + nextSectionY, 0.4F);
    }
}

void GLInventory::generateArmorDetails(std::shared_ptr<const Item> item, float yPosition)
{
    const auto *armor = dynamic_cast<const ArmorItem*>(item.get());
    if (armor) {
        const auto &equipment = m_glPlayer->getEquipment();
        bool showAttackLossSection = false;
        float attackLoss = 0.0F;
        auto currentDefenseGain = [armor, equipment]() {
            switch (armor->getSlotInBodyPart())
            {
            case ArmorBodyPart::SecondaryHand:
                return equipment.getSecondaryHandType() != SecondaryHandType::Armor ? 
                      0.0F : 
                      boost::get<ArmorItem>(equipment.getSecondaryHand().get()).getDefenseGain();
            case ArmorBodyPart::Head:
                return equipment.getHead().has_value() ? equipment.getHead()->getDefenseGain() : 0.0F;
            case ArmorBodyPart::UpperBody:
                return equipment.getUpperBody().has_value() ? equipment.getUpperBody()->getDefenseGain() : 0.0F;
            case ArmorBodyPart::LowerBody:
                return equipment.getLowerBody().has_value() ? equipment.getLowerBody()->getDefenseGain() : 0.0F;
            case ArmorBodyPart::Hands:
                return equipment.getHands().has_value() ? equipment.getHands()->getDefenseGain() : 0.0F;
            case ArmorBodyPart::Feet:
                return equipment.getFeet().has_value() ? equipment.getFeet()->getDefenseGain() : 0.0F;
            default:
                return 0.0F;
            }
        }();
        if (armor->getSlotInBodyPart() == ArmorBodyPart::SecondaryHand && 
            equipment.getSecondaryHandType() == SecondaryHandType::Weapon) {
            showAttackLossSection = true;
            auto currentWeapon = equipment.getSecondaryHand();
            attackLoss = boost::get<WeaponItem>(currentWeapon.get()).getAttackGain() * -1.0F;
        }
        auto gainDifference = armor->getDefenseGain() - currentDefenseGain;
        std::string defenseGainSymbol = gainDifference >=0 ? "+": "";
        std::string defenseValueStr = fmt::format("{0}{1}", defenseGainSymbol, gainDifference);
        GLColor valueColor = [gainDifference]() {
            if (gainDifference > 0) return GLColor::Green;
            if (gainDifference < 0) return GLColor::Red;
            return GLColor::White;
        }();
        generateTwoColumnsLabels("Defense: ", defenseValueStr, yPosition, 0.4F, 
                                 GLColor::White, valueColor);
        float nextSectionY = 25.0F;
        if(showAttackLossSection) {
            generateTwoColumnsLabels("Attack: ", fmt::format("{0}", attackLoss), yPosition + nextSectionY, 0.4F, 
                                     GLColor::White, GLColor::Red);   
            nextSectionY += 25.0F; 
        }
        generateTwoColumnsLabels("Slot in: ", ArmorItem::getBodyPartAsString(armor->getSlotInBodyPart()), yPosition + nextSectionY, 0.4F);
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
    /*m_glTextObjects.emplace_back(GLTextObject({ text, 
                                                { detailBoxXCenter + xOffsetFromCenter - (labelSize.width() / 2.0F), 
                                                    m_windowLocation.y() + m_detailsBoxPosition.y() + yPosition }, 
                                                scale,
                                                color }));*/
    addTextObject({text, 
                   {detailBoxXCenter + xOffsetFromCenter - (labelSize.width() / 2.0F), 
                    m_detailsBoxPosition.y() + yPosition}, 
                    scale,
                    color });
}

void GLInventory::generateDetailLabelXCentered(const std::string &text,
                                               float yPosition,
                                               float scale, 
                                               GLColor color)
{
    addXCenteredTextObject({text, {0.0F, m_detailsBoxPosition.y() + yPosition}, scale, color}, 
                           m_detailsBoxPosition.x(), 
                           DETAILSBOXSIZE.width());
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
    GLPopupWindow::render();
    if (m_inputMode == InventoryInputMode::StatsItemPopup ||
        m_inputMode == InventoryInputMode::ItemPopup ||
        m_inputMode == InventoryInputMode::WeaponOrArmorPopup ||
        m_inputMode == InventoryInputMode::DropItemPopup) {
        m_choicePopup.render();
    }
}

void GLInventory::gameWindowSizeChanged(const Size<> &size)
{
    GLPopupWindow::gameWindowSizeChanged(size);
    m_detailsBoxPosition = { 16.0F, 
                             80.0F };
    m_choicePopup.gameWindowLocationChanged(getWindowCenter());
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
        processEventsListMode(e);
        break;
    case InventoryInputMode::MoveItem:
        processEventsMoveMode(e);
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

void GLInventory::processEventsListMode(SDL_Event &e)
{
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
    else if (e.type == SDL_JOYBUTTONUP && e.jbutton.button == 0) {
        onCloseEvent();
    }
    else if (e.type == SDL_JOYBUTTONUP && e.jbutton.button == 1) {
        inventoryActionButtonPressed();
    }
    for (int i = 0 ; i < SDL_JoystickNumHats(m_joystick) ; i++ ) {
        if (SDL_JoystickGetHat(m_joystick, i) == SDL_HAT_UP) {
            inventoryMoveUpPressed();
            return;
        }
        else if (SDL_JoystickGetHat(m_joystick, i) == SDL_HAT_DOWN) {
            inventoryMoveDownPressed();
            return;
        }
        else if (SDL_JoystickGetHat(m_joystick, i) == SDL_HAT_LEFT) {
            inventoryMoveLeftPressed();
            return;
        }
        else if (SDL_JoystickGetHat(m_joystick, i) == SDL_HAT_RIGHT) {
            inventoryMoveRightPressed();
            return;
        }
    }
}

void GLInventory::processEventsMoveMode(SDL_Event &e)
{
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
    else if (e.type == SDL_JOYBUTTONUP && e.jbutton.button == 0) {
        changeMode(InventoryInputMode::List);
    }
    else if (e.type == SDL_JOYBUTTONUP && e.jbutton.button == 1) {
        completeMoveActionButtonPressed();
    }
    for (int i = 0 ; i < SDL_JoystickNumHats(m_joystick) ; i++ ) {
        if (SDL_JoystickGetHat(m_joystick, i) == SDL_HAT_UP) {
            inventoryMoveUpPressed();
            break;
        }
        else if (SDL_JoystickGetHat(m_joystick, i) == SDL_HAT_DOWN) {
            inventoryMoveDownPressed();
            break;
        }
        else if (SDL_JoystickGetHat(m_joystick, i) == SDL_HAT_LEFT) {
            inventoryMoveLeftPressed();
            break;
        }
        else if (SDL_JoystickGetHat(m_joystick, i) == SDL_HAT_RIGHT) {
            inventoryMoveRightPressed();
            break;
        }
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
        if (choice == 0) {
            equipCurrentElement();
        }
        else if (choice == 1) {
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

void GLInventory::equipCurrentElement()
{
    auto item = m_inventory->getItem(m_inventoryCursorPosition);
    auto &equipment = m_glPlayer->getEquipment();
    if (item != nullptr) {
        if (item->getType() == ItemType::Weapon) {
            auto *weapon = dynamic_cast<const WeaponItem*>(item.get());
            if (weapon != nullptr && weapon->getSlotInBodyPart() == WeaponBodyPart::MainHand) {
                auto currentEquipedId = equipment.getMainHand().has_value() ? 
                                        boost::optional<std::string>(equipment.getMainHand()->getId()) : 
                                        boost::none;
                equipment.setMainHand(*weapon);
                completeEquipTransaction(currentEquipedId);
            }
            else if (weapon != nullptr && weapon->getSlotInBodyPart() == WeaponBodyPart::SecondaryHand) {
                auto currentEquipedId = getSecondaryHandEquipId(equipment);
                equipment.setSecondaryHand(VariantEquipment(*weapon));
                completeEquipTransaction(currentEquipedId);
            }
        }
        else if (item->getType() == ItemType::Armor) {
            auto *armor = dynamic_cast<const ArmorItem*>(item.get());
            if (armor != nullptr) {
                boost::optional<std::string> currentEquipedId = boost::none;
                if (armor->getSlotInBodyPart() == ArmorBodyPart::SecondaryHand) {
                    currentEquipedId = getSecondaryHandEquipId(equipment);
                    equipment.setSecondaryHand(VariantEquipment(*armor));
                }
                else if (armor->getSlotInBodyPart() == ArmorBodyPart::Head) {
                    currentEquipedId = getArmorItemEquipId(equipment.getHead());
                    equipment.setHead(*armor);
                }
                else if (armor->getSlotInBodyPart() == ArmorBodyPart::UpperBody) {
                    currentEquipedId = getArmorItemEquipId(equipment.getUpperBody());
                    equipment.setUpperBody(*armor);
                }
                else if (armor->getSlotInBodyPart() == ArmorBodyPart::LowerBody) {
                    currentEquipedId = getArmorItemEquipId(equipment.getLowerBody());
                    equipment.setLowerBody(*armor);
                }
                else if (armor->getSlotInBodyPart() == ArmorBodyPart::Hands) {
                    currentEquipedId = getArmorItemEquipId(equipment.getHands());
                    equipment.setHands(*armor);
                }
                else if (armor->getSlotInBodyPart() == ArmorBodyPart::Feet) {
                    currentEquipedId = getArmorItemEquipId(equipment.getFeet());
                    equipment.setFeet(*armor);
                }
                completeEquipTransaction(currentEquipedId);
            }
        }
    }
}

boost::optional<std::string> GLInventory::getSecondaryHandEquipId(const PlayerEquipment &equipment) const
{
    if (equipment.getSecondaryHandType() == SecondaryHandType::Weapon) {
        return boost::optional<std::string>(boost::get<WeaponItem>(equipment.getSecondaryHand().get()).getId());
    }
    if (equipment.getSecondaryHandType() == SecondaryHandType::Armor) {
        return boost::optional<std::string>(boost::get<ArmorItem>(equipment.getSecondaryHand().get()).getId());
    }
    return boost::none;
}
    
boost::optional<std::string> GLInventory::getArmorItemEquipId(const boost::optional<ArmorItem> &armor) const
{
    return armor.has_value() ? boost::optional<std::string>(armor->getId()) : boost::none;
}

void GLInventory::completeEquipTransaction(const boost::optional<std::string> &currentEquipedId)
{
    if (currentEquipedId.has_value()) {
        m_inventory->replaceItem(m_inventoryCursorPosition, m_itemStore->findItem(currentEquipedId.get()));
    }
    else {
        m_inventory->dropItem(m_inventoryCursorPosition);
    }
    changeMode(InventoryInputMode::List);
}
