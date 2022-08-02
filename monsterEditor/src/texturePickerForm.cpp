#include "texturePickerForm.hpp"
#include "textureUtils.hpp"
#include "qClickableLabel.hpp"
#include <fmt/core.h>
#include <fmt/format.h>
#include <memory>
#include <qimage.h>
#include <qimagereader.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <string>

TexturePickerForm::TexturePickerForm(QWidget *parent,
        const std::string &resourcesPath,
        const TextureContainer &textureContainer)
    : QDialog(parent),
    ui(Ui::texturePickerFormClass()),
    m_resourcesPath(resourcesPath),
    m_controller(textureContainer)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MonsterEditor Icon.png"));
    connectUIActions();
    refreshTextureComboBox();
    refreshZoomDisplayValue();
}

void TexturePickerForm::refreshTextureComboBox()
{
    ui.comboBoxTexture->model()->removeRows(0, ui.comboBoxTexture->count());
    int i = 0;
    for (const auto &texture : m_controller.getTextureNames()) {
        ui.comboBoxTexture->insertItem(i, texture.c_str());
        i++;
    }
}

void TexturePickerForm::connectUIActions()
{
    connect(ui.pushButtonCancel,
            &QPushButton::clicked,
            this,
            &TexturePickerForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK,
            &QPushButton::clicked,
            this,
            &TexturePickerForm::onPushButtonOKClick);
    connect(ui.labelImageTexture,
            &QClickableLabel::onMouseReleaseEvent,
            this,
            &TexturePickerForm::onLabelImageTextureMouseReleaseEvent);
    connect(ui.comboBoxTexture,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &TexturePickerForm::onComboBoxTextureCurrentIndexChanged);
    connect(ui.horizontalSliderZoom,
            &QSlider::valueChanged,
            this,
            &TexturePickerForm::onHorizontalSliderZoomChanged);
}

void TexturePickerForm::setCurrentSelection(const std::string &textureName,
        int textureIndex)
{
    for (int i = 0; i < ui.comboBoxTexture->count(); i++) {
        if (ui.comboBoxTexture->itemText(i).toStdString() == textureName) {
            ui.comboBoxTexture->setCurrentIndex(i);
        }
    }
    ui.spinBoxTextureIndex->setValue(textureIndex);
    displaySelectedTile(textureName, textureIndex);
}

PickerResult TexturePickerForm::getResult() const
{
    return PickerResult {
        ui.lineEditTextureName->text().toStdString(),
        ui.spinBoxTextureIndex->value()
    };
}

void TexturePickerForm::onPushButtonCancelClick()
{
    reject();
}

void TexturePickerForm::onPushButtonOKClick()
{
    if (!ui.lineEditTextureName->text().trimmed().isEmpty()) {
        accept();
    }
}

void TexturePickerForm::onLabelImageTextureMouseReleaseEvent(QMouseEvent *event)
{
    int comboBoxTextureCurrentIndex { ui.comboBoxTexture->currentIndex() };
    auto textureName { ui.comboBoxTexture->itemText(comboBoxTextureCurrentIndex).toStdString() };
    if (m_controller.isTextureExist(textureName)) {
        setWindowTitle(fmt::format("{}, {}", event->pos().x(), event->pos().y()).c_str());
        float zoomRatio = static_cast<float>(ui.horizontalSliderZoom->value()) / 100.0F;
        int index = m_controller.getTextureIndexFromPosition(Point(event->pos().x(),
                                                                   event->pos().y()),
                                                             textureName,
                                                             zoomRatio);
        ui.spinBoxTextureIndex->setValue(index);
        displaySelectedTile(textureName, index);
    }
}

void TexturePickerForm::displayTexture(const int zoomPercentage)
{
    auto scaledImage = m_loadedTexture->scaled(m_loadedTexture->width() * zoomPercentage / 100,
                                               m_loadedTexture->height() * zoomPercentage / 100,
                                               Qt::KeepAspectRatio,
                                               Qt::FastTransformation);
    ui.labelImageTexture->setPixmap(scaledImage);
    ui.labelImageTexture->setFixedSize(scaledImage.width(), scaledImage.height());
}

void TexturePickerForm::displaySelectedTile(const std::string &textureName,
                                            int textureIndex)
{
    if (m_loadedTexture != nullptr) {
        auto imagePart { m_controller.getTextureTileImageFromTexture(m_loadedTexture.get(),
                textureIndex,
                textureName) };
        imagePart = imagePart.scaled(ui.labelSelectedTexture->maximumSize(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);
        ui.labelSelectedTexture->setPixmap(imagePart);
    }
}

void TexturePickerForm::refreshZoomDisplayValue()
{
    ui.labelZoomValue->setText(fmt::format("{}%", ui.horizontalSliderZoom->value()).c_str());
}

void TexturePickerForm::onComboBoxTextureCurrentIndexChanged()
{
    //Find the selected texture
    auto textureName = ui.comboBoxTexture->itemText(ui.comboBoxTexture->currentIndex()).toStdString();
    if (m_controller.isTextureExist(textureName)) {
        QImageReader reader(m_controller.getTextureFileName(m_resourcesPath, textureName).c_str());
        const QImage image { reader.read() };
        m_loadedTexture = std::make_shared<QPixmap>(QPixmap::fromImage(image));
        displayTexture(ui.horizontalSliderZoom->value());
        ui.lineEditTextureName->setText(textureName.c_str());
    }
    else {
        ui.labelImageTexture->clear();
        ui.lineEditTextureName->clear();
    }
    ui.spinBoxTextureIndex->setValue(0);
    ui.labelSelectedTexture->clear();
}

void TexturePickerForm::onHorizontalSliderZoomChanged(int value)
{
    if (m_loadedTexture != nullptr) {
        displayTexture(value);
    }
    refreshZoomDisplayValue();
}
