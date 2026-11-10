#include <QtGlobal>
#include <QPainter>
#include <QSignalBlocker>
#include <QDrag>
#include <algorithm>
#include <fmt/format.h>
#include <qnamespace.h>
#include <string>
#include "components/tilePropsComponent.hpp"
#include "pickerToolSelection.hpp"
#include "point.hpp"
#include "textureSelectionDockWidget.hpp"
#include "textureUtils.hpp"
#include "textureStampMimeData.hpp"

using commoneditor::ui::TextureUtils;
using thewarrior::models::Point;

TextureSelectionDockWidget::TextureSelectionDockWidget(QWidget *parent,
        MainForm_GLComponent *glComponent)
    : QClosableDockWidget(parent),
      ui(Ui::TextureSelectionDockWidget()),
      m_glComponent(glComponent) {
      ui.setupUi(this);
      ui.labelImageTexture->installEventFilter(this);
      ui.labelImageTexture->setToolTip("In Multi mode, drag to select tiles, then drag the selection onto the map. Click outside the map preview to place it; Escape cancels.");
      connectUIActions();
}

void TextureSelectionDockWidget::connectUIActions() {
    connect(ui.checkBoxDisplayGrid, &QCheckBox::toggled, this, [this](bool enabled) {
        updateTexturePreview();
        emit displayGridChanged(enabled);
    });
    connect(ui.pushButtonSelectedTextureClear, &QPushButton::clicked, this, &TextureSelectionDockWidget::onPushButtonSelectedTextureClearClick);
    connect(ui.pushButtonSelectedObjectClear, &QPushButton::clicked, this, &TextureSelectionDockWidget::onPushButtonSelectedObjectClearClick);
    connect(ui.labelImageTexture, &QClickableLabel::onMouseReleaseEvent, this, &TextureSelectionDockWidget::onLabelImageTextureMouseReleaseEvent);
    connect(ui.comboBoxTexture, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &TextureSelectionDockWidget::onComboBoxTextureCurrentIndexChanged);
    connect(m_glComponent, &MainForm_GLComponent::pickerToolTileSelected, this, &TextureSelectionDockWidget::onPickerToolTileSelected);
    for (auto *radio : {ui.radioButtonTexture, ui.radioButtonObject,
                       ui.radioButtonMultiTexture, ui.radioButtonMultiObject}) {
        connect(radio, &QRadioButton::toggled, this, [this]() {
            m_selectingAtlas = false;
            m_draggingAtlasSelection = false;
            updateTexturePreview();
        });
    }
}

void TextureSelectionDockWidget::refreshTextureList() {
    const QString selectedTexture = ui.comboBoxTexture->currentText();
    {
        const QSignalBlocker blocker(ui.comboBoxTexture);
        ui.comboBoxTexture->model()->removeRows(0, ui.comboBoxTexture->count());
        int index {0};
        for (const auto &texture : m_glComponent->getTextures()) {
            ui.comboBoxTexture->insertItem(index, texture.getName().c_str());
            index++;
        }
        const int selectedIndex = ui.comboBoxTexture->findText(selectedTexture, Qt::MatchExactly);
        if (selectedIndex >= 0) {
            ui.comboBoxTexture->setCurrentIndex(selectedIndex);
        }
    }
    displaySelectedTextureImage();
}

void TextureSelectionDockWidget::setDisplayGrid(bool enabled) {
    const QSignalBlocker blocker(ui.checkBoxDisplayGrid);
    ui.checkBoxDisplayGrid->setChecked(enabled);
    displaySelectedTextureImage();
}

void TextureSelectionDockWidget::displaySelectedTextureImage() {
    m_atlasSelection = {};
    m_selectingAtlas = false;
    m_draggingAtlasSelection = false;
    // Find the selected texture
    auto texture { m_glComponent->getTextureByName(ui.comboBoxTexture->itemText(ui.comboBoxTexture->currentIndex()).toStdString()) };
    if (texture.has_value()) {
        QImageReader reader(fmt::format("{0}/textures/{1}", m_glComponent->getResourcesPath(), texture->get().getFilename()).c_str());
        const QImage image { reader.read() };
        m_texturePixmap = QPixmap::fromImage(image);
        ui.labelImageTexture->setFixedSize(image.width(), image.height());
        updateTexturePreview();
    } else {
        m_texturePixmap = QPixmap();
        ui.labelImageTexture->clear();
        m_glComponent->clearLastSelectedTexture();
        m_glComponent->clearLastSelectedObject();
        ui.labelSelectedTexture->clear();
        ui.labelSelectedObject->clear();
    }
}

bool TextureSelectionDockWidget::isMultiAtlasMode() const {
    return ui.radioButtonMultiTexture->isChecked() || ui.radioButtonMultiObject->isChecked();
}

QPoint TextureSelectionDockWidget::atlasTileAt(QPoint position) const {
    const auto texture = m_glComponent->getTextureByName(ui.comboBoxTexture->currentText().toStdString());
    if (!texture || m_texturePixmap.isNull()) {
        return {-1, -1};
    }
    const int tileWidth = texture->get().getTileWidth();
    const int tileHeight = texture->get().getTileHeight();
    if (tileWidth <= 0 || tileHeight <= 0) {
        return {-1, -1};
    }
    const int columns = std::min(m_texturePixmap.width(), texture->get().getWidth()) / tileWidth;
    const int rows = std::min(m_texturePixmap.height(), texture->get().getHeight()) / tileHeight;
    if (columns <= 0 || rows <= 0) {
        return {-1, -1};
    }
    return {std::clamp(position.x(), 0, columns * tileWidth - 1) / tileWidth,
            std::clamp(position.y(), 0, rows * tileHeight - 1) / tileHeight};
}

void TextureSelectionDockWidget::updateTexturePreview() {
    const auto texture = m_glComponent->getTextureByName(ui.comboBoxTexture->currentText().toStdString());
    if (!texture || m_texturePixmap.isNull()) {
        return;
    }
    QPixmap preview = m_texturePixmap;
    const int tileWidth = texture->get().getTileWidth();
    const int tileHeight = texture->get().getTileHeight();
    if (tileWidth > 0 && tileHeight > 0) {
        QPainter painter(&preview);
        if (ui.checkBoxDisplayGrid->isChecked()) {
            painter.setPen(QPen(Qt::white, 1));
            for (int x = 0; x < preview.width(); x += tileWidth) {
                painter.drawLine(x, 0, x, preview.height() - 1);
            }
            for (int y = 0; y < preview.height(); y += tileHeight) {
                painter.drawLine(0, y, preview.width() - 1, y);
            }
            painter.drawRect(0, 0, preview.width() - 1, preview.height() - 1);
        }
        if (isMultiAtlasMode() && !m_atlasSelection.isEmpty()) {
            painter.setPen(QPen(Qt::green, 2));
            painter.drawRect(m_atlasSelection.x() * tileWidth + 1, m_atlasSelection.y() * tileHeight + 1,
                             m_atlasSelection.width() * tileWidth - 2, m_atlasSelection.height() * tileHeight - 2);
        }
    }
    ui.labelImageTexture->setPixmap(preview);
}

bool TextureSelectionDockWidget::eventFilter(QObject *watched, QEvent *event) {
    if (watched != ui.labelImageTexture || !isMultiAtlasMode()) {
        return QClosableDockWidget::eventFilter(watched, event);
    }
    if (event->type() == QEvent::MouseButtonPress) {
        auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() != Qt::LeftButton) {
            return true;
        }
        const QPoint tile = atlasTileAt(mouse->pos());
        if (tile.x() < 0) {
            return true;
        }
        m_atlasPressPosition = mouse->pos();
        m_draggingAtlasSelection = m_atlasSelection.contains(tile);
        m_selectingAtlas = !m_draggingAtlasSelection;
        if (m_selectingAtlas) {
            m_atlasSelectionStart = tile;
            m_atlasSelection = QRect(tile, tile);
            updateTexturePreview();
        }
        return true;
    }
    if (event->type() == QEvent::MouseMove) {
        auto *mouse = static_cast<QMouseEvent *>(event);
        if (m_draggingAtlasSelection
                && (mouse->pos() - m_atlasPressPosition).manhattanLength() >= QApplication::startDragDistance()) {
            m_draggingAtlasSelection = false;
            startAtlasDrag();
        } else if (m_selectingAtlas) {
            const QPoint tile = atlasTileAt(mouse->pos());
            if (tile.x() >= 0) {
                m_atlasSelection = QRect(QPoint(std::min(m_atlasSelectionStart.x(), tile.x()),
                                               std::min(m_atlasSelectionStart.y(), tile.y())),
                                         QPoint(std::max(m_atlasSelectionStart.x(), tile.x()),
                                                std::max(m_atlasSelectionStart.y(), tile.y())));
                updateTexturePreview();
            }
        }
        return true;
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        m_selectingAtlas = false;
        m_draggingAtlasSelection = false;
        return true;
    }
    return QClosableDockWidget::eventFilter(watched, event);
}

void TextureSelectionDockWidget::startAtlasDrag() {
    const auto texture = m_glComponent->getTextureByName(ui.comboBoxTexture->currentText().toStdString());
    if (!texture || m_atlasSelection.isEmpty()) {
        return;
    }
    const auto stamp = mapeditor::controllers::makeTextureStamp(texture->get(),
            m_atlasSelection.x(), m_atlasSelection.y(), m_atlasSelection.width(), m_atlasSelection.height(),
            ui.radioButtonMultiObject->isChecked());
    if (stamp.indices.empty()) {
        return;
    }
    const QPoint anchor = atlasTileAt(m_atlasPressPosition) - m_atlasSelection.topLeft();
    QDrag drag(ui.labelImageTexture);
    drag.setMimeData(new TextureStampMimeData(stamp, anchor));
    const QRect pixels(m_atlasSelection.x() * texture->get().getTileWidth(),
                       m_atlasSelection.y() * texture->get().getTileHeight(),
                       m_atlasSelection.width() * texture->get().getTileWidth(),
                       m_atlasSelection.height() * texture->get().getTileHeight());
    drag.setPixmap(m_texturePixmap.copy(pixels));
    drag.setHotSpot(m_atlasPressPosition - pixels.topLeft());
    drag.exec(Qt::CopyAction);
}

void TextureSelectionDockWidget::onPushButtonSelectedTextureClearClick() {
    m_glComponent->clearLastSelectedTexture();
    ui.labelSelectedTexture->clear();
}

void TextureSelectionDockWidget::onPushButtonSelectedObjectClearClick() {
    m_glComponent->clearLastSelectedObject();
    ui.labelSelectedObject->clear();
}

void TextureSelectionDockWidget::onLabelImageTextureMouseReleaseEvent(QMouseEvent *event) {
    if (isMultiAtlasMode()) {
        return;
    }
    int comboBoxTextureCurrentIndex { ui.comboBoxTexture->currentIndex() };
    std::string textureName { ui.comboBoxTexture->itemText(comboBoxTextureCurrentIndex).toStdString() };
    auto texture { m_glComponent->getTextureByName(textureName) };
    if (texture.has_value()) {
        std::string name { textureName };
        int index = TextureUtils::getTextureIndexFromPosition(Point(event->pos().x(), event->pos().y()), texture->get());
        // Display the selected texture or object on the selected image
        auto qpixmap = m_texturePixmap;
        auto imagePart { TextureUtils::getTextureTileImageFromTexture(&qpixmap, index, texture->get()) };
        if (ui.radioButtonTexture->isChecked()) {
            m_glComponent->setLastSelectedTexture(name, index);
            ui.labelSelectedTexture->setPixmap(imagePart);
        } else {
            m_glComponent->setLastSelectedObject(name, index);
            ui.labelSelectedObject->setPixmap(imagePart);
        }
    }
}

void TextureSelectionDockWidget::onComboBoxTextureCurrentIndexChanged() {
    displaySelectedTextureImage();
}

void TextureSelectionDockWidget::onPickerToolTileSelected(const PickerToolSelection &selection) {
    // Find the texture in the texture comboBox
    auto texture { m_glComponent->getTextureByName(selection.textureName) };
    if (texture.has_value()) {
        ui.comboBoxTexture->setCurrentText(selection.textureName.c_str());
        auto qpixmap = m_texturePixmap;
        auto imagePart { TextureUtils::getTextureTileImageFromTexture(&qpixmap,
                selection.textureIndex,
                texture->get()), };
        m_glComponent->setLastSelectedTexture(selection.textureName, selection.textureIndex);
        ui.labelSelectedTexture->setPixmap(imagePart);
    }
    auto textureObj { m_glComponent->getTextureByName(selection.objectTextureName) };
    if (textureObj.has_value()) {
        ui.comboBoxTexture->setCurrentText(selection.objectTextureName.c_str());
        auto qpixmapObj = m_texturePixmap;
        auto imagePartObj { TextureUtils::getTextureTileImageFromTexture(&qpixmapObj,
                selection.objectTextureIndex,
                textureObj->get()), };
        m_glComponent->setLastSelectedObject(selection.objectTextureName, selection.objectTextureIndex);
        ui.labelSelectedObject->setPixmap(imagePartObj);
    }
}
