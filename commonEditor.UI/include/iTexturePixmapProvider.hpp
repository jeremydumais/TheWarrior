#pragma once

#include <qpixmap.h>
#include <string>

namespace commoneditor::ui {

class ITexturePixmapProvider {
 public:
     virtual ~ITexturePixmapProvider() = default;
     virtual QPixmap loadPixmap(const std::string &path) = 0;
};

}  // namespace commoneditor::ui
