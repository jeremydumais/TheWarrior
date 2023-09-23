#pragma once

#include "glShaderProgram.hpp"
#include "inventory.hpp"
#include "itemStore.hpp"
#include "glColor.hpp"
#include "glChoicePopup.hpp"
#include "glFormService.hpp"
#include "glObjectService.hpp"
#include "glPlayer.hpp"
#include "glPopupWindow.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include "inputDevicesState.hpp"
#include "point.hpp"
#include "size.hpp"
#include "texture.hpp"
#include <boost/optional.hpp>
#include <GL/glew.h>
#include <SDL2/SDL_events.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace thewarrior::ui {

enum InventoryInputMode
{
    List,
    ItemPopup,
    StatsItemPopup,
    WeaponOrArmorPopup,
    MoveItem,
    DropItemPopup
};

class GLInventory : public GLPopupWindow
{
public:
    GLInventory();
    virtual ~GLInventory() = default;
    void initialize(const std::string &resourcePath,
                    std::shared_ptr<GLPlayer> glPlayer,
                    std::shared_ptr<GLTextService> textService,
                    std::shared_ptr<thewarrior::models::ItemStore> itemStore,
                    const std::map<std::string, unsigned int> *texturesGLItemStore,
                    std::shared_ptr<InputDevicesState> inputDevicesState);
    void setInventory(std::shared_ptr<thewarrior::models::Inventory> inventory);
    void update();
    void generateGLInventory();
    void render();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
private:
    std::shared_ptr<thewarrior::models::Inventory> m_inventory = nullptr;
    std::shared_ptr<GLPlayer> m_glPlayer = nullptr;
    size_t m_inventoryCursorPosition = 0;
    InventoryInputMode m_inputMode = InventoryInputMode::List;
    size_t m_inventoryMoveSrc = 0;
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore;
    GLuint m_glDetailsIconTextureId;
    thewarrior::models::Point<float> m_detailsBoxPosition = { 1.0F, 1.0F };
    GLTexture m_slotsGLTexture = { thewarrior::models::Texture(thewarrior::models::TextureInfo{ "emptySlot", "item_slot.png", 768, 256, 256, 256 }), 0 };
    GLTexture m_inventoryIconsGLTexture = { thewarrior::models::Texture(thewarrior::models::TextureInfo{ "inventory", "inventory.png", 96, 32, 32, 32 }), 0 };
    thewarrior::ui::GLChoicePopup m_choicePopup;
    std::shared_ptr<InputDevicesState> m_inputDevicesState = nullptr;
    Uint64 lastMoveUpTicks = 0;
    Uint64 lastMoveDownTicks = 0;
    Uint64 lastMoveLeftTicks = 0;
    Uint64 lastMoveRightTicks = 0;
    void generateSlots();
    void generateDetailsInfo();
    void generateWeaponDetails(std::shared_ptr<const thewarrior::models::Item> item, float yPosition);
    void generateArmorDetails(std::shared_ptr<const thewarrior::models::Item> item, float yPosition);
    void generateStatsItemDetails(std::shared_ptr<const thewarrior::models::Item> item, float yPosition);
    void generateDetailLabel(const std::string &text,
                             float xOffsetFromCenter,
                             float yPosition,
                             float scale,
                             GLColor color = GLColor::White);
    void generateTwoColumnsLabels(const std::string &label,
                                  const std::string &value,
                                  float yPosition,
                                  float scale,
                                  GLColor colorLabel = GLColor::White,
                                  GLColor colorValue = GLColor::White);
    void generateDetailLabelXCentered(const std::string &text,
                                      float yPosition,
                                      float scale,
                                      GLColor color = GLColor::White);
    thewarrior::models::Point<float> getRowAndColFromInventoryIndex(size_t index) const;
    void changeMode(InventoryInputMode mode);
    void updateListMode();
    void updateMoveMode();
    void updateInventoryMoveKeys();
    void inventoryMoveUpPressed();
    void inventoryMoveDownPressed();
    void inventoryMoveLeftPressed();
    void inventoryMoveRightPressed();
    void inventoryActionButtonPressed();
    void itemActionPopupClicked(size_t choice);
    void itemActionPopupCanceled();
    void completeMoveActionButtonPressed();
    void prepareMoveItemMode();
    void prepareDropItemPopup();
    void equipCurrentElement();
    boost::optional<std::string> getSecondaryHandEquipId(const thewarrior::models::PlayerEquipment &equipment) const;
    boost::optional<std::string> getArmorItemEquipId(const boost::optional<thewarrior::models::ArmorItem> &armor) const;
    void completeEquipTransaction(const boost::optional<std::string> &currentEquipedId);
};

} // namespace thewarrior::ui
