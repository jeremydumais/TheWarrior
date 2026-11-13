#include "editSingleTextureForm.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include <fmt/format.h>
#include <qslider.h>
#include <memory>
#include <utility>
#include "editTextureFormController.hpp"
#include "errorMessage.hpp"

using commoneditor::ui::ErrorMessage;
using commoneditor::ui::EditTextureFormController;
using commoneditor::ui::TextureDTO;

EditSingleTextureForm::EditSingleTextureForm(QWidget *parent,
        const std::string &resourcesPath,
        std::unique_ptr<TextureDTO> originalTexture)
    : QDialog(parent),
    ui(Ui::editSingleTextureFormClass()),
    m_isEditMode(originalTexture != nullptr),
    m_controller(EditTextureFormController(std::move(originalTexture),
                m_emptyNameVector,
                resourcesPath)) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/GameDataEditor Icon.png"));
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditSingleTextureForm::onPushButtonOK);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditSingleTextureForm::reject);
    connect(ui.pushButtonOpenFilename, &QPushButton::clicked, this, &EditSingleTextureForm::onPushButtonOpenFilenameClick);
    connect(ui.spinBoxTileWidth, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &EditSingleTextureForm::onSpinBoxTileWidthValueChanged);
    connect(ui.spinBoxTileHeight, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &EditSingleTextureForm::onSpinBoxTileHeightValueChanged);
    connect(ui.horizontalSliderZoom, &QSlider::valueChanged, this, &EditSingleTextureForm::onHorizontalSliderZoomChanged);
    connect(ui.pushButtonGridBlack, &QPushButton::clicked, this, &EditSingleTextureForm::onPushButtonGridBlackClick);
    connect(ui.pushButtonGridWhite, &QPushButton::clicked, this, &EditSingleTextureForm::onPushButtonGridWhiteClick);

    if (!m_isEditMode) {
        this->setWindowTitle("Add texture");
        m_controller.setName("ItemsTile");

    } else {
        this->setWindowTitle("Edit texture");
        loadExistingItemToForm();
    }
    refreshZoomDisplayValue();
}

std::unique_ptr<TextureDTO> EditSingleTextureForm::getTextureInfo() const {
    return m_controller.createTextureDTOFromFields();
}

void EditSingleTextureForm::loadExistingItemToForm() {
    const auto &texture = m_controller.getOriginalTexture();
    ui.lineEditFilename->setText(texture.filename.c_str());
    ui.labelImageWidth->setText(QString::number(texture.width));
    ui.labelImageHeight->setText(QString::number(texture.height));
    ui.spinBoxTileWidth->setValue(texture.tileWidth);
    ui.spinBoxTileHeight->setValue(texture.tileHeight);
    m_controller.loadTextureImage(texture.filename);
    refreshTextureImage();
}

void EditSingleTextureForm::refreshTextureImage() {
    const auto pixmap = m_controller.getTextureToDisplay();
    if (pixmap != nullptr) {
    ui.labelImage->setPixmap(*pixmap);
    ui.labelImage->setFixedSize(pixmap->width(), pixmap->height());
    } else {
        ui.labelImage->clear();
        ui.labelImage->setFixedSize(0, 0);
    }
}

void EditSingleTextureForm::refreshZoomDisplayValue() {
    ui.labelZoomValue->setText(fmt::format("{}%", ui.horizontalSliderZoom->value()).c_str());
}

void EditSingleTextureForm::onPushButtonOK() {
    auto textureInfo = m_controller.createTextureDTOFromFields();

    if (!m_controller.validateTextureOperation(std::move(textureInfo))) {
        ErrorMessage::show(m_controller.getLastError());
        return;
    }
    accept();
}

void EditSingleTextureForm::onPushButtonOpenFilenameClick() {
    QString fullFilePath { QFileDialog::getOpenFileName(this,
            tr("Open Texture"),
            m_controller.getResourcesPath().c_str(),
            tr("Images (*.png)")) };
    QFileInfo fileInfo(fullFilePath);
    std::string filename { fileInfo.fileName().toStdString() };
    ui.lineEditFilename->setText(filename.c_str());
    if (auto result = m_controller.loadTextureImage(filename); result.Success) {
        ui.labelImageWidth->setText(QString::number(result.TextureWidth));
        ui.labelImageHeight->setText(QString::number(result.TextureHeight));
        refreshTextureImage();
    } else {
        ErrorMessage::show(m_controller.getLastError());
        ui.lineEditFilename->clear();
    }
}

void EditSingleTextureForm::onSpinBoxTileWidthValueChanged(int value) {
    m_controller.setTileWidth(value);
    refreshTextureImage();
}

void EditSingleTextureForm::onSpinBoxTileHeightValueChanged(int value) {
    m_controller.setTileHeight(value);
    refreshTextureImage();
}

void EditSingleTextureForm::onHorizontalSliderZoomChanged(int value) {
    m_controller.setZoom(value);
    refreshZoomDisplayValue();
    refreshTextureImage();
}

void EditSingleTextureForm::onPushButtonGridBlackClick() {
    m_controller.setGridColor(QColor::fromRgb(0, 0, 0));
    refreshTextureImage();
}

void EditSingleTextureForm::onPushButtonGridWhiteClick() {
    m_controller.setGridColor(QColor::fromRgb(255, 255, 255));
    refreshTextureImage();
}
