#include "texturePickerForm.hpp"
#include "textureUtils.hpp"
#include "qClickableLabel.hpp"
#include <fmt/format.h>
#include <qimage.h>
#include <qimagereader.h>
#include <qpixmap.h>
#include <string>

using namespace commoneditor::ui;
using namespace itemeditor::controllers;

TexturePickerForm::TexturePickerForm(QWidget *parent,
        const std::string &resourcesPath,
        const TextureContainer &textureContainer)
    : QDialog(parent),
    ui(Ui::texturePickerFormClass()),
    m_resourcesPath(resourcesPath),
    m_controller(textureContainer)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/ItemEditor Icon.png"));
    connectUIActions();
    refreshTextureComboBox();
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
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &TexturePickerForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &TexturePickerForm::onPushButtonOKClick);
    connect(ui.labelImageTexture, &QClickableLabel::onMouseReleaseEvent, this, &TexturePickerForm::onLabelImageTextureMouseReleaseEvent);
    connect(ui.comboBoxTexture, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &TexturePickerForm::onComboBoxTextureCurrentIndexChanged);
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
    std::string textureName { ui.comboBoxTexture->itemText(comboBoxTextureCurrentIndex).toStdString() };
    if (m_controller.isTextureExist(textureName)) {
        int index = m_controller.getTextureIndexFromPosition(Point(event->pos().x(), event->pos().y()),
                textureName);
        ui.spinBoxTextureIndex->setValue(index);
        displaySelectedTile(textureName, index);
    }
}

void TexturePickerForm::displaySelectedTile(const std::string &textureName,
        int textureIndex)
{
    auto pixmap = TextureUtils::getTexturePixmapFromLabel(ui.labelImageTexture);
    auto imagePart { m_controller.getTextureTileImageFromTexture(&pixmap,
            textureIndex,
            textureName) };
    ui.labelSelectedTexture->setPixmap(imagePart);
}

void TexturePickerForm::onComboBoxTextureCurrentIndexChanged()
{
    //Find the selected texture
    auto textureName = ui.comboBoxTexture->itemText(ui.comboBoxTexture->currentIndex()).toStdString();
    if (m_controller.isTextureExist(textureName)) {
        QImageReader reader(m_controller.getTextureFileName(m_resourcesPath, textureName).c_str());
        const QImage image { reader.read() };
        ui.labelImageTexture->setFixedSize(image.width(), image.height());
        ui.labelImageTexture->setPixmap(QPixmap::fromImage(image));
        ui.lineEditTextureName->setText(textureName.c_str());
    }
    else {
        ui.labelImageTexture->clear();
        ui.lineEditTextureName->clear();
    }
    ui.spinBoxTextureIndex->setValue(0);
    ui.labelSelectedTexture->clear();
}
