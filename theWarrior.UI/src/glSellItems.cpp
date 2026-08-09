#include "glSellItems.hpp"
#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <climits>
#include <fmt/format.h>

using namespace thewarrior::models;

namespace thewarrior::ui {

namespace {
constexpr std::size_t Columns = 7;
constexpr float SlotSize = 70.0F;
constexpr float SlotSpacing = 8.0F;
constexpr int GoldIcon = 0;
constexpr int AttackIcon = 1;
constexpr int DefenseIcon = 2;
}

GLSellItems::GLSellItems() : GLPopupWindow({1020.0F, 570.0F}) {
    m_confirmationDialog.confirmed.connect([this]() { completeSale(); });
    m_confirmationDialog.canceled.connect([this]() { cancelSale(); });
}

void GLSellItems::initialize(const std::string &resourcePath,
                             std::shared_ptr<GLPlayer> glPlayer,
                             std::shared_ptr<GLTextService> textService,
                             std::shared_ptr<ItemStore> itemStore,
                             const std::map<std::string, unsigned int> *texturesGLItemStore,
                             std::shared_ptr<InputDevicesState> inputDevicesState) {
    GLPopupWindow::initialize("Sell Items", resourcePath, textService);
    m_glPlayer = std::move(glPlayer);
    m_itemStore = std::move(itemStore);
    m_texturesGLItemStore = texturesGLItemStore;
    m_inputDevicesState = std::move(inputDevicesState);
    m_textureService.loadTexture(m_slotsGLTexture);
    m_textureService.loadTexture(m_shopIconsGLTexture);
    m_confirmationDialog.initShader(m_shaderProgram);
    m_confirmationDialog.initialize(resourcePath, std::move(textService), m_itemStore,
                                    texturesGLItemStore, m_inputDevicesState, &m_shopIconsGLTexture);
}

void GLSellItems::open(const MerchantInventory *merchantInventory) {
    m_merchantInventory = merchantInventory;
    m_cursorPosition = 0;
    m_confirmationDisplayed = false;
    generateGLElements();
}

unsigned int GLSellItems::getSellPrice(const Item &item) const {
    if (m_merchantInventory != nullptr) {
        const auto priceOverride = m_merchantInventory->getPriceOverridebyItemId(item.getId());
        if (priceOverride && priceOverride->sellPrice) {
            return *priceOverride->sellPrice;
        }
    }
    return item.getDefaultSellPrice();
}

void GLSellItems::update() {
    if (m_confirmationDisplayed) {
        m_confirmationDialog.update();
        return;
    }
    constexpr Uint64 Delay = 110;
    const auto now = SDL_GetTicks64();
    if (now - m_lastMoveTicks > Delay) {
        if (m_inputDevicesState->getUpPressed()) {
            moveCursor(0, -1);
        } else if (m_inputDevicesState->getDownPressed()) {
             moveCursor(0, 1);
        } else if (m_inputDevicesState->getLeftPressed()) { 
            moveCursor(-1, 0);
        } else if (m_inputDevicesState->getRightPressed()) { 
            moveCursor(1, 0);
        } else { 
            m_lastMoveTicks = 0;
        }
        if (m_inputDevicesState->isADirectionKeyPressed()) {
            m_lastMoveTicks = now;
        }
    }
    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        requestSale();
    }
    else if (m_inputDevicesState->getButtonBState() == InputElementState::Released) {
        onCloseEvent();
    }
}

void GLSellItems::moveCursor(int x, int y) {
    const int current = static_cast<int>(m_cursorPosition);
    const int target = current + x + (y * static_cast<int>(Columns));
    if (target >= 0 && target < INVENTORY_MAX &&
        (x == 0 || target / static_cast<int>(Columns) == current / static_cast<int>(Columns))) {
        m_cursorPosition = static_cast<std::size_t>(target);
        generateGLElements();
    }
}

void GLSellItems::requestSale() {
    const auto item = m_glPlayer->getInventory()->getItem(m_cursorPosition);
    if (!item) {
        return;
    }
    m_confirmationDialog.show(item, getSellPrice(*item));
    m_confirmationDisplayed = true;
}

void GLSellItems::completeSale() {
    const auto item = m_glPlayer->getInventory()->getItem(m_cursorPosition);
    if (item) {
        const auto price = getSellPrice(*item);
        if (m_glPlayer->getInventory()->dropItem(m_cursorPosition)) {
            m_glPlayer->addGold(static_cast<int>(std::min(price, static_cast<unsigned int>(INT_MAX))));
        }
    }
    m_confirmationDisplayed = false;
    generateGLElements();
}

void GLSellItems::cancelSale() {
    m_confirmationDisplayed = false;
    generateGLElements();
}

void GLSellItems::render() {
    GLPopupWindow::render();
    if (m_confirmationDisplayed) {
         m_confirmationDialog.render();
    }
}

void GLSellItems::generateGLElements() {
    GLPopupWindow::generateGLElements();
    addWindowPanel({15.0F, 75.0F}, {205.0F, 450.0F}, 0);
    addWindowPanel({230.0F, 75.0F}, {565.0F, 450.0F}, 0);
    addWindowPanel({805.0F, 125.0F}, {190.0F, 145.0F}, 0);
    const auto stats = m_glPlayer->getStats();
    generateQuad(m_glObjects, {750.0F, 18.0F}, {28.0F, 28.0F}, &m_shopIconsGLTexture.texture, GoldIcon, m_shopIconsGLTexture.glTextureId);
    addTextObject({fmt::format("{}", m_glPlayer->getGold()), {782.0F, 41.0F}, 0.34F});
    generateQuad(m_glObjects, {845.0F, 18.0F}, {28.0F, 28.0F}, &m_shopIconsGLTexture.texture, AttackIcon, m_shopIconsGLTexture.glTextureId);
    addTextObject({fmt::format("{}", stats.attack), {877.0F, 41.0F}, 0.34F});
    generateQuad(m_glObjects, {925.0F, 18.0F}, {28.0F, 28.0F}, &m_shopIconsGLTexture.texture, DefenseIcon, m_shopIconsGLTexture.glTextureId);
    addTextObject({fmt::format("{}", stats.defense), {957.0F, 41.0F}, 0.34F});
    addXCenteredTextObject({"Details", {0.0F, 110.0F}, 0.42F}, 15.0F, 205.0F);

    for (std::size_t index = 0; index < INVENTORY_MAX; ++index) {
        const auto col = static_cast<float>(index % Columns);
        const auto row = static_cast<float>(index / Columns);
        const Point<float> position{244.0F + (col * (SlotSize + SlotSpacing)), 88.0F + (row * (SlotSize + SlotSpacing))};
        generateQuad(m_glObjects, position, {SlotSize, SlotSize}, &m_slotsGLTexture.texture,
                     index == m_cursorPosition ? 1 : 0, m_slotsGLTexture.glTextureId);
        const auto item = m_glPlayer->getInventory()->getItem(index);
        if (item) {
            const auto texture = m_itemStore->getTextureContainer().getTextureByName(item->getTextureName());
            const auto textureId = m_texturesGLItemStore->find(item->getTextureName());
            if (texture && textureId != m_texturesGLItemStore->end()) {
                generateQuad(m_glObjects, {position.x() + 7.0F, position.y() + 7.0F}, {56.0F, 56.0F},
                             &texture->get(), item->getTextureIndex(), textureId->second);
            }
        }
    }
    const auto selected = m_glPlayer->getInventory()->getItem(m_cursorPosition);
    if (!selected) {
        addXCenteredTextObject({"Select an item", {0.0F, 195.0F}, 0.34F}, 805.0F, 190.0F);
        return;
    }
    const auto texture = m_itemStore->getTextureContainer().getTextureByName(selected->getTextureName());
    const auto textureId = m_texturesGLItemStore->find(selected->getTextureName());
    if (texture && textureId != m_texturesGLItemStore->end()) {
        generateQuad(m_glObjects, {78.0F, 135.0F}, {80.0F, 80.0F}, &texture->get(), selected->getTextureIndex(), textureId->second);
    }
    addXCenteredTextObject({selected->getName(), {0.0F, 250.0F}, 0.37F}, 15.0F, 205.0F);
    const auto wrapped = m_textService->prepareTextForDisplay({180.0F, 110.0F}, selected->getOptionalDescription(), 0.28F);
    std::size_t line = 0;
    for (const auto &text : wrapped.lines) {
        addXCenteredTextObject({text, {0.0F, 285.0F + (static_cast<float>(line++) * 19.0F)}, 0.28F}, 15.0F, 205.0F);
    }
    const auto price = getSellPrice(*selected);
    addTextObject({fmt::format("Sell price: {}", price), {48.0F, 495.0F}, 0.31F});
    generateQuad(m_glObjects, {171.0F, 476.0F}, {25.0F, 25.0F}, &m_shopIconsGLTexture.texture, GoldIcon, m_shopIconsGLTexture.glTextureId);
    addXCenteredTextObject({"You will receive:", {0.0F, 170.0F}, 0.32F}, 805.0F, 190.0F);
    generateQuad(m_glObjects, {840.0F, 195.0F}, {30.0F, 30.0F}, &m_shopIconsGLTexture.texture, GoldIcon, m_shopIconsGLTexture.glTextureId);
    addTextObject({fmt::format("{}", price), {880.0F, 217.0F}, 0.43F, GLColor::Green});
    addXCenteredTextObject({"A: Sell    B: Leave", {0.0F, 320.0F}, 0.31F}, 805.0F, 190.0F);
}

void GLSellItems::gameWindowSizeChanged(const Size<> &size) {
    GLPopupWindow::gameWindowSizeChanged(size);
    m_confirmationDialog.gameWindowSizeChanged(size);
    generateGLElements();
}

}  // namespace thewarrior::ui
