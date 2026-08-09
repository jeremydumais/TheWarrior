#pragma once

#include <GL/glew.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "glPlayer.hpp"
#include "glPurchaseConfirmationDialog.hpp"
#include "glPopupWindow.hpp"
#include "inputDevicesState.hpp"
#include "itemStore.hpp"
#include "merchantInventory.hpp"
#include "playerStats.hpp"

namespace thewarrior::ui {

class GLMerchantShop : public GLPopupWindow {
 public:
    GLMerchantShop();
    ~GLMerchantShop() override = default;
    void initialize(const std::string &resourcePath,
                    std::shared_ptr<GLPlayer> glPlayer,
                    std::shared_ptr<GLTextService> textService,
                    std::shared_ptr<thewarrior::models::ItemStore> itemStore,
                    const std::map<std::string, unsigned int> *texturesGLItemStore,
                    std::shared_ptr<InputDevicesState> inputDevicesState);
    bool open(const thewarrior::models::MerchantInventory &inventory);
    void update();
    void render();
    void generateGLElements();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);

 private:
    std::shared_ptr<GLPlayer> m_glPlayer;
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore = nullptr;
    std::shared_ptr<InputDevicesState> m_inputDevicesState;
    const thewarrior::models::MerchantInventory *m_inventory = nullptr;
    std::vector<std::shared_ptr<const thewarrior::models::Item>> m_items;
    std::size_t m_cursorPosition = 0;
    std::string m_statusMessage;
    GLPurchaseConfirmationDialog m_confirmationDialog;
    GLTexture m_shopIconsGLTexture = {
        thewarrior::models::Texture(thewarrior::models::TextureInfo {
            "weaponArmorShop", "weaponArmorShop.png", 192, 32, 32, 32}), 0};
    bool m_confirmationDisplayed = false;
    Uint64 m_lastMoveTicks = 0;

    unsigned int getBuyPrice(const thewarrior::models::Item &item) const;
    void moveSelection(int direction);
    void buySelectedItem();
    void confirmPurchase();
    void confirmationCanceled();
    void generateDetails(const thewarrior::models::Item &item, unsigned int price);
    thewarrior::models::PlayerStats getProjectedStats(const thewarrior::models::Item &item) const;
    void generateStatComparison(const std::string &label,
                                float currentValue,
                                float projectedValue,
                                float yPosition);
};

}  // namespace thewarrior::ui
