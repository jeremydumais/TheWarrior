#pragma once

#include <memory>
#include <string>
#include "iTexturePixmapProvider.hpp"

namespace commoneditor::ui {

class FileTexturePixmapProvider final : public ITexturePixmapProvider {
 public:
     std::shared_ptr<QPixmap> loadPixmap(const std::string &path) override;
    ~FileTexturePixmapProvider() override;
};

}  // namespace commoneditor::ui
