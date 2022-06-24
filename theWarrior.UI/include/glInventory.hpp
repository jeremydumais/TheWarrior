#pragma once

#include "glShaderProgram.hpp"
#include "inventory.hpp"
#include "itemStore.hpp"
#include "IShaderService.hpp"
#include "glColor.hpp"
#include "glFormService.hpp"
#include "glChoicePopup.hpp"
#include "glObjectService.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include "point.hpp"
#include "size.hpp"
#include "texture.hpp"
#include <GL/glew.h>
#include <SDL2/SDL_events.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

#define INVENTORY_WINDOW_OBJ_MAX 8

enum InventoryInputMode 
{
    List,
    ItemPopup,
    StatsItemPopup,
    WeaponOrArmorPopup,
    MoveItem,
    DropItemPopup
};

class GLInventory : public IShaderService
{
public:
    GLInventory();
    ~GLInventory() = default;
    bool initShader(const std::string &vertexShaderFileName,
                    const std::string &fragmentShaderFileName) override;
    void initialize(const std::string &resourcePath,
                    std::shared_ptr<GLTextService> textService,
                    std::shared_ptr<ItemStore> itemStore,
                    const std::map<std::string, unsigned int> *texturesGLItemStore);
    const std::string &getLastError() const;
    void setInventory(std::shared_ptr<Inventory> inventory);
    void processEvents(SDL_Event &e);
    void generateGLInventory();
    void render();
    void gameWindowSizeChanged(const Size<> &size);
private:
    std::shared_ptr<Inventory> m_inventory;
    Point<float> m_inventoryWindowLocation;
    Size<float> m_gameWindowSize;
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
    std::string m_lastError;
    size_t m_inventoryCursorPosition;
    InventoryInputMode m_inputMode;
    size_t m_inventoryMoveSrc;
    std::shared_ptr<GLFormService> m_glFormService;
    std::shared_ptr<GLTextService> m_textService;
    GLTextureService m_textureService;  
    std::shared_ptr<ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore;
    GLObject m_glInventoryWindow;
    const Size<float> m_inventorySize;
    GLTextObject m_glTitle;
    GLTextObject m_glTextDetails;
    std::vector<GLObject> m_glSlots;
    GLTexture m_slotsGLTexture;
    std::vector<GLObject> m_glInventoryItems;
    std::vector<GLObject> m_inventoryWindowObjects;
    GLTexture m_inventoryWindowGLTexture;
    GLChoicePopup m_choicePopup;
    Point<float> getRowAndColFromInventoryIndex(size_t index) const;
    void changeMode(InventoryInputMode mode);
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
};