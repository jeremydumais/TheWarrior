#pragma once

#include <qpixmap.h>
#include <memory>
#include <string>

namespace commoneditor::ui {

class ITexturePixmapProvider {
 public:
     virtual ~ITexturePixmapProvider() = default;
     virtual std::shared_ptr<QPixmap> loadPixmap(const std::string &path) = 0;
};

}  // namespace commoneditor::ui
