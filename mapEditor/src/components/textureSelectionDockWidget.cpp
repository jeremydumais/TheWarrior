#include "textureSelectionDockWidget.hpp"
#include <QtGlobal>
#include <fmt/format.h>
#include <qnamespace.h>
#include <string>
#include "components/tilePropsComponent.hpp"
#include "pickerToolSelection.hpp"
#include "point.hpp"
#include "textureUtils.hpp"

using commoneditor::ui::TextureUtils;
using thewarrior::models::Point;

TextureSelectionDockWidget::TextureSelectionDockWidget(QWidget *parent,
        MainForm_GLComponent *glComponent)
    : QClosableDockWidget(parent),
      ui(Ui::TextureSelectionDockWidget()),
      m_glComponent(glComponent) {
      ui.setupUi(this);
      connectUIActions();
}

void TextureSelectionDockWidget::connectUIActions() {
    connect(ui.pushButtonSelectedTextureClear, &QPushButton::clicked, this, &TextureSelectionDockWidget::onPushButtonSelectedTextureClearClick);
    connect(ui.pushButtonSelectedObjectClear, &QPushButton::clicked, this, &TextureSelectionDockWidget::onPushButtonSelectedObjectClearClick);
    connect(ui.labelImageTexture, &QClickableLabel::onMouseReleaseEvent, this, &TextureSelectionDockWidget::onLabelImageTextureMouseReleaseEvent);
    connect(ui.comboBoxTexture, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &TextureSelectionDockWidget::onComboBoxTextureCurrentIndexChanged);
    connect(m_glComponent, &MainForm_GLComponent::pickerToolTileSelected, this, &TextureSelectionDockWidget::onPickerToolTileSelected);
}

void TextureSelectionDockWidget::refreshTextureList() {
    ui.comboBoxTexture->model()->removeRows(0, ui.comboBoxTexture->count());
    int index {0};
    for (const auto &texture : m_glComponent->getTextures()) {
        ui.comboBoxTexture->insertItem(index, texture.getName().c_str());
        index++;
    }
    displaySelectedTextureImage();
}

void TextureSelectionDockWidget::displaySelectedTextureImage() {
    // Find the selected texture
    auto texture { m_glComponent->getTextureByName(ui.comboBoxTexture->itemText(ui.comboBoxTexture->currentIndex()).toStdString()) };
    if (texture.has_value()) {
        QImageReader reader(fmt::format("{0}/textures/{1}", m_glComponent->getResourcesPath(), texture->get().getFilename()).c_str());
        const QImage image { reader.read() };
        ui.labelImageTexture->setFixedSize(image.width(), image.height());
        ui.labelImageTexture->setPixmap(QPixmap::fromImage(image));
    } else {
        ui.labelImageTexture->clear();
        m_glComponent->clearLastSelectedTexture();
        m_glComponent->clearLastSelectedObject();
        ui.labelSelectedTexture->clear();
        ui.labelSelectedObject->clear();
    }
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
    int comboBoxTextureCurrentIndex { ui.comboBoxTexture->currentIndex() };
    std::string textureName { ui.comboBoxTexture->itemText(comboBoxTextureCurrentIndex).toStdString() };
    auto texture { m_glComponent->getTextureByName(textureName) };
    if (texture.has_value()) {
        std::string name { textureName };
        int index = TextureUtils::getTextureIndexFromPosition(Point(event->pos().x(), event->pos().y()), texture->get());
        // Display the selected texture or object on the selected image
        auto qpixmap = TextureUtils::getTexturePixmapFromLabel(ui.labelImageTexture);
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
        auto qpixmap = TextureUtils::getTexturePixmapFromLabel(ui.labelImageTexture);
        auto imagePart { TextureUtils::getTextureTileImageFromTexture(&qpixmap,
                selection.textureIndex,
                texture->get()) };
        m_glComponent->setLastSelectedTexture(selection.textureName, selection.textureIndex);
        ui.labelSelectedTexture->setPixmap(imagePart);
    }
    auto textureObj { m_glComponent->getTextureByName(selection.objectTextureName) };
    if (textureObj.has_value()) {
        ui.comboBoxTexture->setCurrentText(selection.objectTextureName.c_str());
        auto qpixmapObj = TextureUtils::getTexturePixmapFromLabel(ui.labelImageTexture);
        auto imagePartObj { TextureUtils::getTextureTileImageFromTexture(&qpixmapObj,
                selection.objectTextureIndex,
                textureObj->get()) };
        m_glComponent->setLastSelectedObject(selection.objectTextureName, selection.objectTextureIndex);
        ui.labelSelectedObject->setPixmap(imagePartObj);
    }
}
