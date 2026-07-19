#pragma once

#include <string>

namespace thewarrior::models {

enum class ChestContentType {
    Nothing,
    Item,
    Gold
};

namespace openchestproperty {
    inline constexpr const char *ContentType = "contentType";
    inline constexpr const char *ItemId = "itemId";
    inline constexpr const char *GoldAmount = "goldAmount";
    inline constexpr const char *OpenedTextureIndex = "objectTextureIndexOpenedChest";
}  // namespace thewarrior::models::openchestproperty

class OpenChestActionProperties {
 public:
    ChestContentType getContentType() const;
    const std::string &getItemId() const;
    unsigned int getGoldAmount() const;
    int getOpenedTextureIndex() const;

    void setContentType(ChestContentType type);
    void setItemId(const std::string& itemId);
    void setGoldAmount(unsigned int amount);
    void setOpenedTextureIndex(int textureIndex);

 private:
    ChestContentType m_contentType = ChestContentType::Nothing;
    std::string m_itemId;
    unsigned int m_goldAmount = 0;
    int m_openedTextureIndex = -1;
};

}  // namespace thewarrior::models