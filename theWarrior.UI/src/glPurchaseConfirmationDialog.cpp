#include <fmt/format.h>
#include "glPurchaseConfirmationDialog.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui {

namespace {
constexpr int GoldIcon = 0;
constexpr float DialogWidth = 474.0F;
}

GLPurchaseConfirmationDialog::GLPurchaseConfirmationDialog()
    : GLPopupWindow({DialogWidth, 455.0F}) {}

void GLPurchaseConfirmationDialog::initialize(
    const std::string &resourcePath,
    std::shared_ptr<GLTextService> textService,
    std::shared_ptr<ItemStore> itemStore,
    const std::map<std::string, unsigned int> *texturesGLItemStore,
    std::shared_ptr<InputDevicesState> inputDevicesState,
    const GLTexture *shopIconsTexture) {
    GLPopupWindow::initialize("Confirm Purchase", resourcePath, std::move(textService));
    m_itemStore = std::move(itemStore);
    m_texturesGLItemStore = texturesGLItemStore;
    m_inputDevicesState = std::move(inputDevicesState);
    m_shopIconsTexture = shopIconsTexture;
}

void GLPurchaseConfirmationDialog::show(std::shared_ptr<const Item> item, unsigned int price) {
    m_item = std::move(item);
    m_price = price;
    m_yesSelected = true;
    generateGLElements();
}

void GLPurchaseConfirmationDialog::update() {
    if (m_inputDevicesState->getLeftPressed() && !m_yesSelected) {
        m_yesSelected = true;
        generateGLElements();
    } else if (m_inputDevicesState->getRightPressed() && m_yesSelected) {
        m_yesSelected = false;
        generateGLElements();
    }

    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        if (m_yesSelected) {
            confirmed();
        } else {
            canceled();
        }
    } else if (m_inputDevicesState->getButtonBState() == InputElementState::Released) {
        canceled();
    }
}

void GLPurchaseConfirmationDialog::generateGLElements() {
    GLPopupWindow::generateGLElements();
    if (m_item == nullptr) {
        return;
    }

    addXCenteredTextObject({"Are you sure you want to buy", {0.0F, 85.0F}, 0.4F}, 0.0F, DialogWidth);
    addWindowPanel({25.0F, 105.0F}, {424.0F, 145.0F}, 0);

    const auto texture = m_itemStore->getTextureContainer().getTextureByName(m_item->getTextureName());
    const auto textureId = m_texturesGLItemStore->find(m_item->getTextureName());
    if (texture.has_value() && textureId != m_texturesGLItemStore->end()) {
        generateQuad(m_glObjects, {42.0F, 135.0F}, {90.0F, 90.0F},
                     &texture->get(), m_item->getTextureIndex(), textureId->second);
    }
    addTextObject({m_item->getName(), {150.0F, 145.0F}, 0.36F, GLColor::Red});
    if (!m_item->getOptionalDescription().empty()) {
        const auto description = m_textService->prepareTextForDisplay(
            {275.0F, 70.0F}, m_item->getOptionalDescription(), 0.27F);
        std::size_t line = 0;
        for (const auto &text : description.lines) {
            addTextObject({text, {150.0F, 182.0F + (static_cast<float>(line) * 18.0F)}, 0.27F});
            ++line;
        }
    }
    addXCenteredTextObject({"You will pay:", {0.0F, 305.0F}, 0.38F}, 0.0F, DialogWidth);
    const auto priceText = fmt::format("{}", m_price);
    const auto priceSize = m_textService->getTextSize(priceText, 0.55F);
    const float priceGroupWidth = 38.0F + priceSize.width();
    const float priceGroupX = (DialogWidth - priceGroupWidth) / 2.0F;
    generateQuad(m_glObjects, {priceGroupX, 315.0F}, {34.0F, 34.0F},
                 &m_shopIconsTexture->texture, GoldIcon, m_shopIconsTexture->glTextureId);
    addTextObject({priceText, {priceGroupX + 44.0F, 340.0F}, 0.55F, GLColor::Red});

    generateBoxQuad(m_glObjects, {45.0F, 370.0F}, {150.0F, 60.0F},
                    &m_windowGLTexture.texture, m_yesSelected ? 17 : 0, m_windowGLTexture.glTextureId);
    generateBoxQuad(m_glObjects, {279.0F, 370.0F}, {150.0F, 60.0F},
                    &m_windowGLTexture.texture, m_yesSelected ? 0 : 17, m_windowGLTexture.glTextureId);
    addXCenteredTextObject({"Yes", {0.0F, 410.0F}, 0.48F}, 45.0F, 150.0F);
    addXCenteredTextObject({"No", {0.0F, 410.0F}, 0.48F}, 279.0F, 150.0F);
}

void GLPurchaseConfirmationDialog::gameWindowSizeChanged(const Size<> &size) {
    GLPopupWindow::gameWindowSizeChanged(size);
    if (m_item != nullptr) {
        generateGLElements();
    }
}

}  // namespace thewarrior::ui
