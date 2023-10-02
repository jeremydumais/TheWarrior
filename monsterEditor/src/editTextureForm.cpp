#include "editTextureForm.hpp"
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <fmt/format.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qslider.h>
#include <memory>
#include <utility>
#include "editTextureFormController.hpp"
#include "errorMessage.hpp"
#include "textureDTO.hpp"

using commoneditor::ui::ErrorMessage;
using monstereditor::controllers::EditTextureFormController;
using monstereditor::controllers::TextureDTO;

EditTextureForm::EditTextureForm(QWidget *parent,
        const std::string &resourcesPath,
        std::unique_ptr<TextureDTO> originalTexture,
        const std::vector<std::string> &allTextureNames)
    : QDialog(parent),
    ui(Ui::editTextureFormClass()),
    m_isEditMode(originalTexture != nullptr),
    m_controller(EditTextureFormController(std::move(originalTexture), allTextureNames)),
    m_resourcesPath(resourcesPath) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/ItemEditor Icon.png"));
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditTextureForm::onPushButtonOK);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditTextureForm::reject);
    connect(ui.pushButtonOpenFilename, &QPushButton::clicked, this, &EditTextureForm::onPushButtonOpenFilenameClick);
    connect(ui.horizontalSliderZoom, &QSlider::valueChanged, this, &EditTextureForm::onHorizontalSliderZoomChanged);

    if (!m_isEditMode) {
        this->setWindowTitle("Add texture");
    } else {
        this->setWindowTitle("Edit texture");
        loadExistingItemToForm();
    }
    refreshZoomDisplayValue();
}

std::unique_ptr<TextureDTO> EditTextureForm::getTextureInfo() const {
    return createTextureDTOFromFields();
}

void EditTextureForm::loadExistingItemToForm() {
    const auto &texture = m_controller.getOriginalTexture();
    ui.lineEditName->setText(texture.name.c_str());
    ui.lineEditFilename->setText(texture.filename.c_str());
    ui.spinBoxWidth->setValue(texture.width);
    ui.spinBoxHeight->setValue(texture.height);
    ui.spinBoxTileWidth->setValue(texture.tileWidth);
    ui.spinBoxTileHeight->setValue(texture.tileHeight);
    loadTextureImage(texture.filename);
}

MonsterEditorTextureLoadResult EditTextureForm::loadTextureImage(const std::string &filename) {
    QImage image;
    std::string imageFullPath { fmt::format("{0}/textures/{1}", m_resourcesPath, filename) };
    if (!image.load(imageFullPath.c_str())) {
        ErrorMessage::show(fmt::format("Unable to load the image {0}", imageFullPath));
        ui.lineEditFilename->clear();
        return { false, 0, 0 };
    }
    m_loadedTexture = std::make_shared<QPixmap>(QPixmap::fromImage(image));
    m_loadedTextureWithTiles = std::make_shared<QPixmap>(*m_loadedTexture);
    drawTilesOnImage();
    ui.labelImage->setFixedSize(image.width(), image.height());
    ui.labelImage->setPixmap(*m_loadedTextureWithTiles.get());
    return { true, image.width(), image.height() };
}

void EditTextureForm::drawTilesOnImage() {
    //QPainter painter(m_loadedTextureWithTiles.get());
    //painter.drawRect(5, 5, 30, 30);
}

void EditTextureForm::refreshZoomDisplayValue() {
    ui.labelZoomValue->setText(fmt::format("{}%", ui.horizontalSliderZoom->value()).c_str());
}

void EditTextureForm::onPushButtonOK() {
    auto textureInfo = createTextureDTOFromFields();

    if (!m_controller.validateTextureOperation(std::move(textureInfo))) {
        ErrorMessage::show(m_controller.getLastError());
        return;
    }
    accept();
}

void EditTextureForm::onPushButtonOpenFilenameClick() {
    QString fullFilePath { QFileDialog::getOpenFileName(this,
            tr("Open Texture"),
            m_resourcesPath.c_str(),
            tr("Images (*.png)")) };
    QFileInfo fileInfo(fullFilePath);
    std::string filename { fileInfo.fileName().toStdString() };
    ui.lineEditFilename->setText(filename.c_str());
    if (auto result = loadTextureImage(filename); result.Success) {
        ui.spinBoxWidth->setValue(result.TextureWidth);
        ui.spinBoxHeight->setValue(result.TextureHeight);
    }
}


void EditTextureForm::onHorizontalSliderZoomChanged(int value) {
    if (m_loadedTexture != nullptr) {
        auto scaledImage = m_loadedTextureWithTiles->scaled(m_loadedTexture->width() * value / 100,
                                                   m_loadedTexture->height() * value / 100,
                                                   Qt::KeepAspectRatio,
                                                   Qt::FastTransformation);
        ui.labelImage->setPixmap(scaledImage);
        ui.labelImage->setFixedSize(scaledImage.width(), scaledImage.height());
    }
    refreshZoomDisplayValue();
}

std::unique_ptr<TextureDTO> EditTextureForm::createTextureDTOFromFields() const {
    return std::unique_ptr<TextureDTO>(new TextureDTO {
            ui.lineEditName->text().toStdString(),
            ui.lineEditFilename->text().toStdString(),
            ui.spinBoxWidth->value(),
            ui.spinBoxHeight->value(),
            ui.spinBoxTileWidth->value(),
            ui.spinBoxTileHeight->value()
            });
}
