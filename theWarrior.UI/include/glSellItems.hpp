#pragma once

#include <map>
#include <memory>
#include "glPlayer.hpp"
#include "glPopupWindow.hpp"
#include "glSaleConfirmationDialog.hpp"
#include "inputDevicesState.hpp"
#include "itemStore.hpp"
#include "merchantInventory.hpp"

namespace thewarrior::ui {

class GLSellItems : public GLPopupWindow {
 public:
    GLSellItems();
    void initialize(const std::string &resourcePath,
                    std::shared_ptr<GLPlayer> glPlayer,
                    std::shared_ptr<GLTextService> textService,
                    std::shared_ptr<thewarrior::models::ItemStore> itemStore,
                    const std::map<std::string, unsigned int> *texturesGLItemStore,
                    std::shared_ptr<InputDevicesState> inputDevicesState);
    void open(const thewarrior::models::MerchantInventory *merchantInventory);
    void update();
    void render();
    void generateGLElements();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);

 private:
    std::shared_ptr<GLPlayer> m_glPlayer;
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore = nullptr;
    std::shared_ptr<InputDevicesState> m_inputDevicesState;
    const thewarrior::models::MerchantInventory *m_merchantInventory = nullptr;
    std::size_t m_cursorPosition = 0;
    bool m_confirmationDisplayed = false;
    GLSaleConfirmationDialog m_confirmationDialog;
    GLTexture m_slotsGLTexture = { thewarrior::models::Texture(thewarrior::models::TextureInfo {
        "emptySlot", "item_slot.png", 768, 256, 256, 256}), 0};
    GLTexture m_shopIconsGLTexture = { thewarrior::models::Texture(thewarrior::models::TextureInfo {
        "weaponArmorShop", "weaponArmorShop.png", 192, 32, 32, 32}), 0};
    Uint64 m_lastMoveTicks = 0;
    unsigned int getSellPrice(const thewarrior::models::Item &item) const;
    void moveCursor(int x, int y);
    void requestSale();
    void completeSale();
    void cancelSale();
};

}  // namespace thewarrior::ui
