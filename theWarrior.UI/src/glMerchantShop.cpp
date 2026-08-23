#include "glMerchantShop.hpp"
#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <fmt/format.h>
#include <limits>
#include "armorItem.hpp"
#include "weaponItem.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui {

namespace {
constexpr float ItemIconSize = 48.0F;
constexpr float RowHeight = 58.0F;
constexpr std::size_t VisibleItemCount = 6;
constexpr int GoldIcon = 0;
constexpr int AttackIcon = 1;
constexpr int DefenseIcon = 2;
constexpr int ScrollDownIcon = 3;
constexpr int ScrollUpIcon = 4;
constexpr int RightArrowIcon = 5;

std::string formatStatValue(float value, bool showPositiveSign = false) {
    auto formatted = showPositiveSign
        ? fmt::format("{:+.1f}", value)
        : fmt::format("{:.1f}", value);
    if (formatted.ends_with(".0")) {
        formatted.erase(formatted.size() - 2);
    }
    return formatted;
}
}

GLMerchantShop::GLMerchantShop()
    : GLPopupWindow({1060.0F, 520.0F}) {
    m_confirmationDialog.confirmed.connect([this]() {
        m_confirmationDisplayed = false;
        buySelectedItem();
    });
    m_confirmationDialog.canceled.connect([this]() { confirmationCanceled(); });
}

void GLMerchantShop::initialize(
    const std::string &resourcePath,
    std::shared_ptr<GLPlayer> glPlayer,
    std::shared_ptr<GLTextService> textService,
    std::shared_ptr<ItemStore> itemStore,
    const std::map<std::string, unsigned int> *texturesGLItemStore,
    std::shared_ptr<InputDevicesState> inputDevicesState) {
    GLPopupWindow::initialize("Buy Items", resourcePath, textService);
    m_glPlayer = std::move(glPlayer);
    m_itemStore = std::move(itemStore);
    m_texturesGLItemStore = texturesGLItemStore;
    m_inputDevicesState = std::move(inputDevicesState);
    m_textureService.loadTexture(m_shopIconsGLTexture);
    m_confirmationDialog.initShader(m_shaderProgram);
    m_confirmationDialog.initialize(resourcePath, m_textService, m_itemStore,
                                    m_texturesGLItemStore, m_inputDevicesState,
                                    &m_shopIconsGLTexture);
}

bool GLMerchantShop::open(const MerchantInventory &inventory) {
    m_inventory = &inventory;
    m_items.clear();
    m_cursorPosition = 0;
    m_statusMessage.clear();
    m_confirmationDisplayed = false;

    for (const auto &itemId : inventory.getItemIds()) {
        const auto item = m_itemStore->findItem(itemId);
        if (item != nullptr) {
            m_items.push_back(item);
        }
    }

    setTitle(inventory.getInventoryType() == MerchantInventoryType::WeaponsAndArmors
                 ? "Buy Weapons & Armor"
                 : "Buy Items");
    generateGLElements();
    return !m_items.empty();
}

unsigned int GLMerchantShop::getBuyPrice(const Item &item) const {
    const auto priceOverride = m_inventory->getPriceOverridebyItemId(item.getId());
    if (priceOverride.has_value() && priceOverride->buyPrice.has_value()) {
        return *priceOverride->buyPrice;
    }
    return item.getDefaultBuyPrice();
}

void GLMerchantShop::update() {
    if (m_confirmationDisplayed) {
        m_confirmationDialog.update();
        return;
    }
    constexpr Uint64 SelectionDelay = 110;
    const auto now = SDL_GetTicks64();
    if (m_inputDevicesState->getUpPressed() && now - m_lastMoveTicks > SelectionDelay) {
        moveSelection(-1);
        m_lastMoveTicks = now;
    } else if (m_inputDevicesState->getDownPressed() && now - m_lastMoveTicks > SelectionDelay) {
        moveSelection(1);
        m_lastMoveTicks = now;
    } else if (!m_inputDevicesState->getUpPressed() &&
               !m_inputDevicesState->getDownPressed()) {
        m_lastMoveTicks = 0;
    }

    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        confirmPurchase();
    } else if (m_inputDevicesState->getButtonBState() == InputElementState::Released) {
        onCloseEvent();
    }
}

void GLMerchantShop::render() {
    GLPopupWindow::render();
    if (m_confirmationDisplayed) {
        m_confirmationDialog.render();
    }
}

void GLMerchantShop::confirmPurchase() {
    if (m_items.empty() || m_cursorPosition >= m_items.size()) {
        return;
    }
    const auto &item = m_items[m_cursorPosition];
    const auto price = getBuyPrice(*item);
    if (price > static_cast<unsigned int>(std::numeric_limits<int>::max()) ||
        m_glPlayer->getGold() < 0 ||
        static_cast<unsigned int>(m_glPlayer->getGold()) < price) {
        m_statusMessage = "Not enough gold.";
        generateGLElements();
        return;
    }
    m_confirmationDialog.show(item, price);
    m_confirmationDisplayed = true;
}

void GLMerchantShop::confirmationCanceled() {
    m_confirmationDisplayed = false;
    generateGLElements();
}

void GLMerchantShop::moveSelection(int direction) {
    if (m_items.empty()) {
        return;
    }
    if (direction < 0 && m_cursorPosition > 0) {
        --m_cursorPosition;
    } else if (direction > 0 && m_cursorPosition + 1 < m_items.size()) {
        ++m_cursorPosition;
    } else {
        return;
    }
    m_statusMessage.clear();
    generateGLElements();
}

void GLMerchantShop::buySelectedItem() {
    if (m_items.empty() || m_cursorPosition >= m_items.size()) {
        return;
    }

    const auto &item = m_items[m_cursorPosition];
    const auto price = getBuyPrice(*item);
    if (price > static_cast<unsigned int>(std::numeric_limits<int>::max()) ||
        static_cast<unsigned int>(m_glPlayer->getGold()) < price) {
        m_statusMessage = "Not enough gold.";
        generateGLElements();
        return;
    }
    if (!m_glPlayer->getInventory()->addItem(item)) {
        m_statusMessage = "Your inventory is full.";
        generateGLElements();
        return;
    }

    m_glPlayer->removeGold(static_cast<int>(price));
    m_statusMessage = fmt::format("Purchased {}.", item->getName());
    generateGLElements();
}

void GLMerchantShop::generateGLElements() {
    GLPopupWindow::generateGLElements();
    addWindowPanel({18.0F, 70.0F}, {265.0F, 410.0F}, 0);
    addWindowPanel({295.0F, 70.0F}, {410.0F, 410.0F}, 0);
    addWindowPanel({717.0F, 70.0F}, {325.0F, 330.0F}, 0);

    const auto playerStats = m_glPlayer->getStats();
    generateQuad(m_glObjects, {810.0F, 17.0F}, {28.0F, 28.0F},
                 &m_shopIconsGLTexture.texture, GoldIcon, m_shopIconsGLTexture.glTextureId);
    addTextObject({fmt::format("{}", m_glPlayer->getGold()), {842.0F, 40.0F}, 0.35F});
    generateQuad(m_glObjects, {898.0F, 17.0F}, {28.0F, 28.0F},
                 &m_shopIconsGLTexture.texture, AttackIcon, m_shopIconsGLTexture.glTextureId);
    addTextObject({fmt::format("{}", playerStats.attack), {930.0F, 40.0F}, 0.35F});
    generateQuad(m_glObjects, {975.0F, 17.0F}, {28.0F, 28.0F},
                 &m_shopIconsGLTexture.texture, DefenseIcon, m_shopIconsGLTexture.glTextureId);
    addTextObject({fmt::format("{}", playerStats.defense), {1007.0F, 40.0F}, 0.35F});
    addXCenteredTextObject({"Details", {0.0F, 105.0F}, 0.45F}, 18.0F, 265.0F);
    addXCenteredTextObject({m_inventory->getInventoryType() == MerchantInventoryType::WeaponsAndArmors
                                ? "Compared to equipped"
                                : "Owned in inventory",
                            {0.0F, 105.0F}, 0.35F},
                           717.0F, 325.0F);

    if (m_items.empty()) {
        addXCenteredTextObject({"No items for sale", {0.0F, 250.0F}, 0.4F}, 295.0F, 410.0F);
        return;
    }

    const auto firstVisible = m_cursorPosition >= VisibleItemCount
        ? m_cursorPosition - VisibleItemCount + 1
        : 0;
    const auto lastVisible = std::min(firstVisible + VisibleItemCount, m_items.size());
    if (firstVisible > 0) {
        generateQuad(m_glObjects, {485.0F, 76.0F}, {30.0F, 30.0F},
                     &m_shopIconsGLTexture.texture, ScrollUpIcon, m_shopIconsGLTexture.glTextureId);
    }
    if (lastVisible < m_items.size()) {
        generateQuad(m_glObjects, {485.0F, 443.0F}, {30.0F, 30.0F},
                     &m_shopIconsGLTexture.texture, ScrollDownIcon, m_shopIconsGLTexture.glTextureId);
    }
    for (std::size_t index = firstVisible; index < lastVisible; ++index) {
        const auto &item = m_items[index];
        const auto price = getBuyPrice(*item);
        const auto row = index - firstVisible;
        const float y = 110.0F + (static_cast<float>(row) * RowHeight);
        if (index == m_cursorPosition) {
            generateBoxQuad(m_glObjects, {308.0F, y - 12.0F}, {384.0F, 55.0F},
                            &m_windowGLTexture.texture, 17, m_windowGLTexture.glTextureId);
        }
        const auto texture = m_itemStore->getTextureContainer()
                                 .getTextureByName(item->getTextureName());
        const auto textureId = m_texturesGLItemStore->find(item->getTextureName());
        if (texture.has_value() && textureId != m_texturesGLItemStore->end()) {
            generateQuad(m_glObjects, {315.0F, y - 8.0F}, {ItemIconSize, ItemIconSize},
                         &texture->get(), item->getTextureIndex(), textureId->second);
        }
        const bool affordable = static_cast<unsigned int>(m_glPlayer->getGold()) >= price;
        addTextObject({item->getName(), {375.0F, y + 21.0F}, 0.38F,
                       affordable ? GLColor::White : GLColor::Red});
        addTextObject({fmt::format("{}", price), {610.0F, y + 21.0F}, 0.38F,
                       affordable ? GLColor::White : GLColor::Red});
        generateQuad(m_glObjects, {658.0F, y - 1.0F}, {25.0F, 25.0F},
                     &m_shopIconsGLTexture.texture, GoldIcon, m_shopIconsGLTexture.glTextureId);
    }

    const auto &selected = *m_items[m_cursorPosition];
    generateDetails(selected, getBuyPrice(selected));
    addXCenteredTextObject({m_statusMessage.empty() ? "A: Buy    B: Leave" : m_statusMessage,
                            {0.0F, 455.0F}, 0.35F,
                            m_statusMessage.starts_with("Purchased") ? GLColor::Green : GLColor::White},
                           717.0F, 325.0F);
}

void GLMerchantShop::generateDetails(const Item &item, unsigned int price) {
    const auto texture = m_itemStore->getTextureContainer().getTextureByName(item.getTextureName());
    const auto textureId = m_texturesGLItemStore->find(item.getTextureName());
    if (texture.has_value() && textureId != m_texturesGLItemStore->end()) {
        generateQuad(m_glObjects, {102.0F, 130.0F}, {96.0F, 96.0F},
                     &texture->get(), item.getTextureIndex(), textureId->second);
    }
    addXCenteredTextObject({item.getName(), {0.0F, 257.0F}, 0.4F}, 18.0F, 265.0F);
    if (!item.getOptionalDescription().empty()) {
        const auto wrapped = m_textService->prepareTextForDisplay({240.0F, 100.0F},
                                                                  item.getOptionalDescription(), 0.3F);
        std::size_t line = 0;
        for (const auto &text : wrapped.lines) {
            addXCenteredTextObject({text, {0.0F, 297.0F + (static_cast<float>(line) * 20.0F)}, 0.3F},
                                   18.0F, 265.0F);
            ++line;
        }
    }
    addTextObject({fmt::format("Price: {}", price), {98.0F, 445.0F}, 0.35F});
    generateQuad(m_glObjects, {191.0F, 426.0F}, {25.0F, 25.0F},
                 &m_shopIconsGLTexture.texture, GoldIcon, m_shopIconsGLTexture.glTextureId);

    if (m_inventory->getInventoryType() == MerchantInventoryType::StatsItems) {
        const auto &slots = m_glPlayer->getInventory()->getAllSlots();
        const auto ownedCount = std::count_if(
            slots.begin(), slots.end(), [&item](const auto &ownedItem) {
                return ownedItem != nullptr && ownedItem->getId() == item.getId();
            });
        addXCenteredTextObject({fmt::format("{}", ownedCount), {0.0F, 220.0F}, 0.7F},
                               717.0F, 325.0F);
        return;
    }

    const auto &equipment = m_glPlayer->getEquipment();
    std::string equippedName = "Nothing equipped";
    if (const auto *weapon = dynamic_cast<const WeaponItem *>(&item)) {
        const WeaponItem *equipped = nullptr;
        if (weapon->getSlotInBodyPart() == WeaponBodyPart::MainHand &&
            equipment.getMainHand().has_value()) {
            equipped = &*equipment.getMainHand();
        } else if (weapon->getSlotInBodyPart() == WeaponBodyPart::SecondaryHand &&
                   equipment.getSecondaryHandType() == SecondaryHandType::Weapon) {
            equipped = &boost::get<WeaponItem>(*equipment.getSecondaryHand());
        }
        if (equipped != nullptr) {
            equippedName = equipped->getName();
        }
    } else if (const auto *armor = dynamic_cast<const ArmorItem *>(&item)) {
        const ArmorItem *equipped = nullptr;
        switch (armor->getSlotInBodyPart()) {
        case ArmorBodyPart::Head:
            if (equipment.getHead()) {
                equipped = &*equipment.getHead();
            }
            break;
        case ArmorBodyPart::UpperBody:
            if (equipment.getUpperBody()) {
                equipped = &*equipment.getUpperBody();
            }
            break;
        case ArmorBodyPart::LowerBody:
            if (equipment.getLowerBody()) {
                equipped = &*equipment.getLowerBody();
            }
            break;
        case ArmorBodyPart::Hands:
            if (equipment.getHands()) {
                equipped = &*equipment.getHands();
            }
            break;
        case ArmorBodyPart::Feet:
            if (equipment.getFeet()) {
                equipped = &*equipment.getFeet();
            }
            break;
        case ArmorBodyPart::SecondaryHand:
            if (equipment.getSecondaryHandType() == SecondaryHandType::Armor) {
                equipped = &boost::get<ArmorItem>(*equipment.getSecondaryHand());
            }
            break;
        }
        if (equipped != nullptr) {
            equippedName = equipped->getName();
        }
    }
    addXCenteredTextObject({equippedName, {0.0F, 145.0F}, 0.34F}, 717.0F, 325.0F);
    const auto currentStats = m_glPlayer->getStats();
    const auto projectedStats = getProjectedStats(item);
    generateStatComparison("Attack: ", currentStats.attack, projectedStats.attack, 190.0F);
    generateStatComparison("Defense: ", currentStats.defense, projectedStats.defense, 240.0F);
}

PlayerStats GLMerchantShop::getProjectedStats(const Item &item) const {
    auto projected = m_glPlayer->getStats();
    const auto &equipment = m_glPlayer->getEquipment();
    if (const auto *weapon = dynamic_cast<const WeaponItem *>(&item)) {
        if (weapon->getSlotInBodyPart() == WeaponBodyPart::MainHand) {
            if (equipment.getMainHand()) {
                projected.attack -= equipment.getMainHand()->getAttackGain();
            }
        } else if (equipment.getSecondaryHandType() == SecondaryHandType::Weapon) {
            projected.attack -= boost::get<WeaponItem>(*equipment.getSecondaryHand()).getAttackGain();
        } else if (equipment.getSecondaryHandType() == SecondaryHandType::Armor) {
            projected.defense -= boost::get<ArmorItem>(*equipment.getSecondaryHand()).getDefenseGain();
        }
        projected.attack += weapon->getAttackGain();
    } else if (const auto *armor = dynamic_cast<const ArmorItem *>(&item)) {
        const ArmorItem *equippedArmor = nullptr;
        switch (armor->getSlotInBodyPart()) {
        case ArmorBodyPart::Head: if (equipment.getHead()) equippedArmor = &*equipment.getHead(); break;
        case ArmorBodyPart::UpperBody: if (equipment.getUpperBody()) equippedArmor = &*equipment.getUpperBody(); break;
        case ArmorBodyPart::LowerBody: if (equipment.getLowerBody()) equippedArmor = &*equipment.getLowerBody(); break;
        case ArmorBodyPart::Hands: if (equipment.getHands()) equippedArmor = &*equipment.getHands(); break;
        case ArmorBodyPart::Feet: if (equipment.getFeet()) equippedArmor = &*equipment.getFeet(); break;
        case ArmorBodyPart::SecondaryHand:
            if (equipment.getSecondaryHandType() == SecondaryHandType::Armor) {
                equippedArmor = &boost::get<ArmorItem>(*equipment.getSecondaryHand());
            } else if (equipment.getSecondaryHandType() == SecondaryHandType::Weapon) {
                projected.attack -= boost::get<WeaponItem>(*equipment.getSecondaryHand()).getAttackGain();
            }
            break;
        }
        if (equippedArmor != nullptr) {
            projected.defense -= equippedArmor->getDefenseGain();
        }
        projected.defense += armor->getDefenseGain();
    }
    return projected;
}

void GLMerchantShop::generateStatComparison(const std::string &label,
                                            float currentValue,
                                            float projectedValue,
                                            float yPosition) {
    const float difference = projectedValue - currentValue;
    addTextObject({label, {745.0F, yPosition}, 0.31F});
    addTextObject({formatStatValue(currentValue), {835.0F, yPosition}, 0.31F});
    generateQuad(m_glObjects, {885.0F, yPosition - 22.0F}, {22.0F, 22.0F},
                 &m_shopIconsGLTexture.texture, RightArrowIcon, m_shopIconsGLTexture.glTextureId);
    addTextObject({formatStatValue(projectedValue), {920.0F, yPosition}, 0.31F});
    addTextObject({fmt::format("({})", formatStatValue(difference, true)), {965.0F, yPosition}, 0.28F,
                   difference > 0.0F ? GLColor::Green : difference < 0.0F ? GLColor::Red : GLColor::White});
}

void GLMerchantShop::gameWindowSizeChanged(const Size<> &size) {
    GLPopupWindow::gameWindowSizeChanged(size);
    m_confirmationDialog.gameWindowSizeChanged(size);
    if (m_inventory != nullptr) {
        generateGLElements();
    }
}

}  // namespace thewarrior::ui
