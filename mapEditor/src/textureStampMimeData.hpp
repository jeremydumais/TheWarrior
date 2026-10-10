#pragma once

#include <QMimeData>
#include <QPoint>
#include "textureStamp.hpp"

// Only drags created by this editor can supply a texture stamp.
class TextureStampMimeData : public QMimeData {
 public:
    TextureStampMimeData(const mapeditor::controllers::TextureStamp &value, QPoint anchor)
        : stamp(value), tileAnchor(anchor) {
        setData("application/x-thewarrior-texture-stamp", QByteArray());
    }
    mapeditor::controllers::TextureStamp stamp;
    QPoint tileAnchor;
};
