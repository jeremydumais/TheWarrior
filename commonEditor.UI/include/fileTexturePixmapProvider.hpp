#pragma once

#include <string>
#include "iTexturePixmapProvider.hpp"

namespace commoneditor::ui {

class FileTexturePixmapProvider final : public ITexturePixmapProvider {
 public:
    QPixmap loadPixmap(const std::string &path) override;
    ~FileTexturePixmapProvider() override;
};

}  // namespace commoneditor::ui
