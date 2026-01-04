#include <qchar.h>
#include <qpixmap.h>
#include <memory>
#include <string>
#include "fileTexturePixmapProvider.hpp"

namespace commoneditor::ui {

std::shared_ptr<QPixmap> FileTexturePixmapProvider::loadPixmap(const std::string &path) {
    return std::make_shared<QPixmap>(QString::fromStdString(path));
}

FileTexturePixmapProvider::~FileTexturePixmapProvider() {}

}  // namespace commoneditor::ui
