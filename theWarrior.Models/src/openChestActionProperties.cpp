#include "openChestActionProperties.hpp"

namespace thewarrior::models {

ChestContentType OpenChestActionProperties::getContentType() const {
    return m_contentType;
}

const std::string &OpenChestActionProperties::getItemId() const {
    return m_itemId;
}

unsigned int OpenChestActionProperties::getGoldAmount() const {
    return m_goldAmount;
}

int OpenChestActionProperties::getOpenedTextureIndex() const {
    return m_openedTextureIndex;
}

void OpenChestActionProperties::setContentType(ChestContentType type) {
    m_contentType = type;
}

void OpenChestActionProperties::setItemId(const std::string& itemId) {
    m_itemId = itemId;
}

void OpenChestActionProperties::setGoldAmount(unsigned int amount) {
    m_goldAmount = amount;
}

void OpenChestActionProperties::setOpenedTextureIndex(int textureIndex) {
    m_openedTextureIndex = textureIndex;
}

}  // namespace thewarrior::models
