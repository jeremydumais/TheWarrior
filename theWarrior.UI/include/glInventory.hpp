#pragma once

#include "glShaderProgram.hpp"
#include "inventory.hpp"
#include "itemStore.hpp"
#include "IShaderService.hpp"
#include "glColor.hpp"
#include "glObjectService.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include "point.hpp"
#include "size.hpp"
#include "texture.hpp"
#include <GL/glew.h>
#include <SDL2/SDL_events.h>
#include <array>
#include <map>
#include <memory>
#include <string>

#define INVENTORY_WINDOW_OBJ_MAX 8
#define POPUP_MENU_OBJ_MAX 9

struct GLTextObject
{
    std::string text;
    Point<float> position;
    float scale;
};

enum InventoryInputMode 
{
    List,
    ItemPopup,
    StatsItemPopup,
    WeaponOrArmorPopup,
    MoveItem
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
    std::unique_ptr<GLShaderProgram> m_shaderProgram;
    std::string m_lastError;
    size_t m_inventoryCursorPosition;
    size_t m_popupMenuCursorPosition;
    size_t m_popupMenuItemCount;
    InventoryInputMode m_inputMode;
    size_t m_inventoryMoveSrc;
    std::shared_ptr<GLTextService> m_textService;
    GLTextureService m_textureService;  
    std::shared_ptr<ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore;
    GLObject m_glInventoryWindow;
    const Size<float> m_inventorySize;
    GLTextObject m_glTitle;
    GLTextObject m_glTextDetails;
    GLTextObject m_glPopupMenuUse;
    GLTextObject m_glPopupMenuMove;
    GLTextObject m_glPopupMenuDrop;
    std::array<GLObject, INVENTORY_MAX> m_glSlots;
    GLTexture m_slotsGLTexture;
    std::array<GLObject, INVENTORY_MAX> m_glInventoryItems;
    std::array<GLObject, INVENTORY_WINDOW_OBJ_MAX> m_inventoryWindowObjects;
    std::array<GLObject, POPUP_MENU_OBJ_MAX> m_popupMenuObjects;
    GLTexture m_inventoryWindowGLTexture;
    void generateQuad(GLObject &object, Point<float> location, Size<float> size, const Texture *texture = nullptr, int textureId = 0);
    void generateBoxQuad(std::array<GLObject, INVENTORY_WINDOW_OBJ_MAX>::iterator begin, 
                         Point<float> location, 
                         Size<float> size,
                         const Texture *texture,
                         int textureBeginId);
    Point<float> getRowAndColFromInventoryIndex(size_t index) const;
    void drawQuad(const GLObject &glObject, GLuint textureGLIndex, float transparency = 0.0F);
    void drawText(const GLTextObject &glTextObject, GLColor color = GLColor::White);
    void changeMode(InventoryInputMode mode);
    void inventoryMoveUpPressed();
    void inventoryMoveDownPressed();
    void inventoryMoveLeftPressed();
    void inventoryMoveRightPressed();
    void inventoryActionButtonPressed();
    void popupMenuMoveUpPressed();
    void popupMenuMoveDownPressed();
    void statsItemActionButtonPressed();
    void MoveActionButtonPressed();
};