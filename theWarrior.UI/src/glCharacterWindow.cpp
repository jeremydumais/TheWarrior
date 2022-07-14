#include "glCharacterWindow.hpp"
#include <fmt/format.h>
#include <string>

const float SLOTSIZE = 80.0F;
const float ITEMSIZE = 70.0F;

GLCharacterWindow::GLCharacterWindow()
    : GLPopupWindow({ 800.0F, 600.0F }),
      m_slotsGLTexture({ Texture(TextureInfo{ "emptySlot", "item_slot.png", 768, 256, 256, 256 }), 0 })
{
}

void GLCharacterWindow::initialize(const std::string &resourcePath,
                                   std::shared_ptr<GLPlayer> glPlayer,
                                   std::shared_ptr<GLTextService> textService,
                                   std::shared_ptr<ItemStore> itemStore,
                                   const std::map<std::string, unsigned int> *texturesGLItemStore,
                                   std::shared_ptr<InputDevicesState> inputDevicesState)
{
    GLPopupWindow::initialize("Character", resourcePath, textService);
    m_glPlayer = glPlayer;
    m_itemStore = itemStore;
    m_texturesGLItemStore = texturesGLItemStore;
    m_inputDevicesState = inputDevicesState;
    m_textureService.loadTexture(m_slotsGLTexture);
}

void GLCharacterWindow::update()
{
    if(m_inputDevicesState->getButtonBState() == InputElementState::Released) {
        onCloseEvent();
    }
}

void GLCharacterWindow::generateGLElements()
{
    GLPopupWindow::generateGLElements();
    //Equipments
    auto equipment = m_glPlayer->getEquipment();
    addWindowPanel({20.0F, 80.0F}, {380.0F, 500.0F}, 0);
    addWindowPanel({420.0F, 80.0F}, {360.0F, 500.0F}, 0);
    addWindowPanel({125.0F, 93.0F}, {167.0F, 40.0F}, 17);
    addWindowPanel({488.0F, 93.0F}, {222.0F, 40.0F}, 17);
    addSlot({55.0F, 190.0F}); //Main hand
    addSlot({170.0F, 150.0F}); //Head
    addSlot({285.0F, 190.0F}); //Secondary hand
    addSlot({285.0F, 285.0F}); //Hands
    addSlot({170.0F, 270.0F}); //Upper body
    addSlot({170.0F, 365.0F}); //Lower body
    addSlot({170.0F, 460.0F}); //Feet
    addXCenteredTextObject({"Equipment", {0.0F, 120.0F}, 0.5F}, 20.0F, 380.0F);
    addXCenteredTextObject({"Infos and Stats", {0.0F, 120.0F}, 0.5F}, 420.0F, 360.0F);
    addXCenteredTwoColumnsLabels("Name: ", m_glPlayer->getName(), 170.0F, 0.4F, 420.0F, 360.0F, GLColor::LightGray, GLColor::White);
    addXCenteredTwoColumnsLabels("Class :", "Warrior", 195.0F, 0.3F, 420.0F, 360.0F, GLColor::LightGray, GLColor::White);
    addXCenteredTwoColumnsLabels("Level: ", std::to_string(m_glPlayer->getLevel()), 245.0F, 0.4F, 420.0F, 360.0F, GLColor::LightGray, GLColor::Green);
    const auto stats = m_glPlayer->getStats();
    addXCenteredTwoColumnsLabels("Health: ", fmt::format("{0}/{1}", stats.health, stats.maxHealth), 275.0F, 0.4F, 420.0F, 360.0F, GLColor::LightGray, GLColor::Green);
    addXCenteredTwoColumnsLabels("Total attack: ", fmt::format("{0}", stats.attack), 305.0F, 0.4F, 420.0F, 360.0F, GLColor::LightGray, GLColor::Green);
    addXCenteredTwoColumnsLabels("Total defense: ", fmt::format("{0}", stats.defense), 335.0F, 0.4F, 420.0F, 360.0F, GLColor::LightGray, GLColor::Green);
    //Main hand
    if (!equipment.getMainHand().has_value()) {
        addXCenteredTextObject({"Main\nhand", {0.0F, 225.0F}, 0.3F}, 55.0F, 80.0F);
    }
    else {
        const Item *item = dynamic_cast<const Item *>(equipment.getMainHand().get_ptr());
        addItemToSlot(item, {60.0F, 200.0F});
    }
    //Head
    if (!equipment.getHead().has_value()) {
        addXCenteredTextObject({"Head", {0.0F, 195.0F}, 0.3F}, 170.0F, 80.0F);
    }
    else {
        const Item *item = dynamic_cast<const Item *>(equipment.getHead().get_ptr());
        addItemToSlot(item, {175.0F, 160.0F});
    }
    //Secondary hand
    if (equipment.getSecondaryHandType() == SecondaryHandType::None) {
        addXCenteredTextObject({"Second.\nhand", {0.0F, 225.0F}, 0.3F}, 285.0F, 80.0F);
    }
    else {
        auto itemVariant = equipment.getSecondaryHand().get();
        const Item *item = nullptr;
        if (equipment.getSecondaryHandType() == SecondaryHandType::Weapon) {
            item = dynamic_cast<const Item *>(&boost::get<WeaponItem>(itemVariant));
        }
        else if (equipment.getSecondaryHandType() == SecondaryHandType::Armor) {
            item = dynamic_cast<const Item *>(&boost::get<ArmorItem>(itemVariant));
        }
        addItemToSlot(item, {290.0F, 200.0F});
    }
    //Hands
    if (!equipment.getHands().has_value()) {
        addXCenteredTextObject({"Hands", {0.0F, 330.0F}, 0.3F}, 285.0F, 80.0F);
    }
    else {
        const Item *item = dynamic_cast<const Item *>(equipment.getHands().get_ptr());
        addItemToSlot(item, {290.0F, 295.0F});
    }
    //Upper body
    if (!equipment.getUpperBody().has_value()) {
        addXCenteredTextObject({"Upper\nbody", {0.0F, 305.0F}, 0.3F}, 170.0F, 80.0F);
    }
    else {
        const Item *item = dynamic_cast<const Item *>(equipment.getUpperBody().get_ptr());
        addItemToSlot(item, {175.0F, 280.0F});
    }
    //Lower body
    if (!equipment.getLowerBody().has_value()) {
        addXCenteredTextObject({"Lower\nbody", {0.0F, 400.0F}, 0.3F}, 170.0F, 80.0F);
    }
    else {
        const Item *item = dynamic_cast<const Item *>(equipment.getLowerBody().get_ptr());
        addItemToSlot(item, {175.0F, 375.0F});
    }
    //Feet
    if (!equipment.getFeet().has_value()) {
        addXCenteredTextObject({"Feet", {0.0F, 505.0F}, 0.3F}, 170.0F, 80.0F);
    }
    else {
        const Item *item = dynamic_cast<const Item *>(equipment.getFeet().get_ptr());
        addItemToSlot(item, {175.0F, 470.0F});
    }
}
    
void GLCharacterWindow::addSlot(Point<float> location)
{
    generateQuad(m_glObjects, location, {SLOTSIZE, SLOTSIZE}, &m_slotsGLTexture.texture, 0, m_slotsGLTexture.glTextureId);
}

void GLCharacterWindow::addItemToSlot(const Item *item, Point<float> location)
{
    auto iconTexture = &m_itemStore->getTextureContainer().getTextureByName(item->getTextureName()).value().get(); 
    generateQuad(m_glObjects, 
                    location, 
                    {ITEMSIZE, ITEMSIZE}, 
                    iconTexture, 
                    item->getTextureIndex(), 
                    m_texturesGLItemStore->at(item->getTextureName()));
}

void GLCharacterWindow::render()
{
    GLPopupWindow::render();
}

void GLCharacterWindow::gameWindowSizeChanged(const Size<> &size)
{
    GLPopupWindow::gameWindowSizeChanged(size);
    generateGLElements();
}