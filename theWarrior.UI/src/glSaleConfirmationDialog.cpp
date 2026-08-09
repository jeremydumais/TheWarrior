#include "glSaleConfirmationDialog.hpp"
#include <fmt/format.h>

using namespace thewarrior::models;

namespace thewarrior::ui {

namespace { constexpr int GoldIcon = 0; }

GLSaleConfirmationDialog::GLSaleConfirmationDialog()
    : GLPopupWindow({474.0F, 455.0F}) {}

void GLSaleConfirmationDialog::initialize(
    const std::string &resourcePath,
    std::shared_ptr<GLTextService> textService,
    std::shared_ptr<ItemStore> itemStore,
    const std::map<std::string, unsigned int> *texturesGLItemStore,
    std::shared_ptr<InputDevicesState> inputDevicesState,
    const GLTexture *shopIconsTexture) {
    GLPopupWindow::initialize("Confirm Sale", resourcePath, std::move(textService));
    m_itemStore = std::move(itemStore);
    m_texturesGLItemStore = texturesGLItemStore;
    m_inputDevicesState = std::move(inputDevicesState);
    m_shopIconsTexture = shopIconsTexture;
}

void GLSaleConfirmationDialog::show(std::shared_ptr<const Item> item, unsigned int price) {
    m_item = std::move(item);
    m_price = price;
    m_yesSelected = true;
    generateGLElements();
}

void GLSaleConfirmationDialog::update() {
    if (m_inputDevicesState->getLeftPressed() && !m_yesSelected) {
        m_yesSelected = true;
        generateGLElements();
    } else if (m_inputDevicesState->getRightPressed() && m_yesSelected) {
        m_yesSelected = false;
        generateGLElements();
    }
    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        m_yesSelected ? confirmed() : canceled();
    } else if (m_inputDevicesState->getButtonBState() == InputElementState::Released) {
        canceled();
    }
}

void GLSaleConfirmationDialog::generateGLElements() {
    GLPopupWindow::generateGLElements();
    if (m_item == nullptr) return;
    addXCenteredTextObject({"Are you sure you want to sell", {0.0F, 95.0F}, 0.39F}, 0.0F, 474.0F);
    addXCenteredTextObject({fmt::format("{}?", m_item->getName()), {0.0F, 125.0F}, 0.39F}, 0.0F, 474.0F);
    addWindowPanel({75.0F, 150.0F}, {324.0F, 110.0F}, 0);
    const auto texture = m_itemStore->getTextureContainer().getTextureByName(m_item->getTextureName());
    const auto textureId = m_texturesGLItemStore->find(m_item->getTextureName());
    if (texture && textureId != m_texturesGLItemStore->end()) {
        generateQuad(m_glObjects, {95.0F, 165.0F}, {80.0F, 80.0F},
                     &texture->get(), m_item->getTextureIndex(), textureId->second);
    }
    addTextObject({m_item->getName(), {205.0F, 215.0F}, 0.36F});
    addXCenteredTextObject({"You will receive:", {0.0F, 305.0F}, 0.36F}, 0.0F, 474.0F);
    const auto priceText = fmt::format("{}", m_price);
    const auto priceWidth = m_textService->getTextSize(priceText, 0.48F).width();
    const float groupX = (474.0F - 34.0F - priceWidth) / 2.0F;
    generateQuad(m_glObjects, {groupX, 320.0F}, {30.0F, 30.0F},
                 &m_shopIconsTexture->texture, GoldIcon, m_shopIconsTexture->glTextureId);
    addTextObject({priceText, {groupX + 40.0F, 347.0F}, 0.48F, GLColor::Green});
    generateBoxQuad(m_glObjects, {45.0F, 375.0F}, {150.0F, 55.0F},
                    &m_windowGLTexture.texture, m_yesSelected ? 17 : 0, m_windowGLTexture.glTextureId);
    generateBoxQuad(m_glObjects, {279.0F, 375.0F}, {150.0F, 55.0F},
                    &m_windowGLTexture.texture, m_yesSelected ? 0 : 17, m_windowGLTexture.glTextureId);
    addXCenteredTextObject({"Yes", {0.0F, 412.0F}, 0.42F}, 45.0F, 150.0F);
    addXCenteredTextObject({"No", {0.0F, 412.0F}, 0.42F}, 279.0F, 150.0F);
}

void GLSaleConfirmationDialog::gameWindowSizeChanged(const Size<> &size) {
    GLPopupWindow::gameWindowSizeChanged(size);
    if (m_item) {
        generateGLElements();
    }
}

}  // namespace thewarrior::ui
