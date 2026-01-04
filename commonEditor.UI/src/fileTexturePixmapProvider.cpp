#include <qchar.h>
#include <qpixmap.h>
#include <string>
#include "fileTexturePixmapProvider.hpp"

namespace commoneditor::ui {

QPixmap FileTexturePixmapProvider::loadPixmap(const std::string &path) {
    return QPixmap(QString::fromStdString(path));
}

FileTexturePixmapProvider::~FileTexturePixmapProvider() {}

}  // namespace commoneditor::ui
