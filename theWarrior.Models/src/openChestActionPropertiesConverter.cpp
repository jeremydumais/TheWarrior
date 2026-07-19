#include "openChestActionPropertiesConverter.hpp"
#include <charconv>

namespace thewarrior::models {

OpenChestActionProperties OpenChestActionPropertiesConverter::fromMap(
  const std::map<std::string, std::string>& properties) {
    OpenChestActionProperties result;

    if (const auto it = properties.find(openchestproperty::ContentType); it != properties.end()) {
        int value = 0;
        const auto parseResult = std::from_chars(
            it->second.data(), it->second.data() + it->second.size(), value);
        if (parseResult.ec == std::errc{} &&
            parseResult.ptr == it->second.data() + it->second.size()) {
            switch (value) {
                case static_cast<int>(ChestContentType::Nothing):
                case static_cast<int>(ChestContentType::Item):
                case static_cast<int>(ChestContentType::Gold):
                    result.setContentType(static_cast<ChestContentType>(value));
                    break;
                default:
                    break;
            }
        }
    }
    if (const auto it = properties.find(openchestproperty::ItemId); it != properties.end()) {
        result.setItemId(it->second);
    }
    if (const auto it = properties.find(openchestproperty::GoldAmount); it != properties.end()) {
        unsigned int value = 0;
        if (std::from_chars(it->second.data(), it->second.data() + it->second.size(), value).ec == std::errc{}) {
            result.setGoldAmount(value);
        }
    }
    if (const auto it = properties.find(openchestproperty::OpenedTextureIndex); it != properties.end()) {
        int value = -1;
        if (std::from_chars(it->second.data(), it->second.data() + it->second.size(), value).ec == std::errc{}) {
            result.setOpenedTextureIndex(value);
        }
    }

    return result;
}

std::map<std::string, std::string> OpenChestActionPropertiesConverter::toMap(
  const OpenChestActionProperties& properties) {
    return {
        {openchestproperty::ContentType,
         std::to_string(static_cast<int>(properties.getContentType()))},
        {openchestproperty::ItemId, properties.getItemId()},
        {openchestproperty::GoldAmount, std::to_string(properties.getGoldAmount())},
        {openchestproperty::OpenedTextureIndex,
         std::to_string(properties.getOpenedTextureIndex())}
    };
}

}  // namespace thewarrior::models
