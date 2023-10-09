#include "editTextureForm.hpp"
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

EditTextureForm::EditTextureForm(QWidget *parent,
        const std::string &resourcesPath,
        std::unique_ptr<TextureDTO> originalTexture,
        const std::vector<std::string> &allTextureNames)
    : QDialog(parent),
    ui(Ui::editTextureFormClass()),
    m_isEditMode(originalTexture != nullptr),
    m_controller(EditTextureFormController(std::move(originalTexture),
                allTextureNames,
                resourcesPath)) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/ItemEditor Icon.png"));
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditTextureForm::onPushButtonOK);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditTextureForm::reject);
    connect(ui.lineEditName, &QLineEdit::textChanged, this, &EditTextureForm::onLineEditNameTextChanged);
    connect(ui.pushButtonOpenFilename, &QPushButton::clicked, this, &EditTextureForm::onPushButtonOpenFilenameClick);
    connect(ui.spinBoxTileWidth, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &EditTextureForm::onSpinBoxTileWidthValueChanged);
    connect(ui.spinBoxTileHeight, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &EditTextureForm::onSpinBoxTileHeightValueChanged);
    connect(ui.horizontalSliderZoom, &QSlider::valueChanged, this, &EditTextureForm::onHorizontalSliderZoomChanged);
    connect(ui.pushButtonGridBlack, &QPushButton::clicked, this, &EditTextureForm::onPushButtonGridBlackClick);
    connect(ui.pushButtonGridWhite, &QPushButton::clicked, this, &EditTextureForm::onPushButtonGridWhiteClick);

    if (!m_isEditMode) {
        this->setWindowTitle("Add texture");
    } else {
        this->setWindowTitle("Edit texture");
        loadExistingItemToForm();
    }
    refreshZoomDisplayValue();
}

std::unique_ptr<TextureDTO> EditTextureForm::getTextureInfo() const {
    return m_controller.createTextureDTOFromFields();
}

void EditTextureForm::loadExistingItemToForm() {
    const auto &texture = m_controller.getOriginalTexture();
    ui.lineEditName->setText(texture.name.c_str());
    ui.lineEditFilename->setText(texture.filename.c_str());
    ui.labelImageWidth->setText(QString::number(texture.width));
    ui.labelImageHeight->setText(QString::number(texture.height));
    ui.spinBoxTileWidth->setValue(texture.tileWidth);
    ui.spinBoxTileHeight->setValue(texture.tileHeight);
    m_controller.loadTextureImage(texture.filename);
    refreshTextureImage();
}

void EditTextureForm::refreshTextureImage() {
    const auto pixmap = m_controller.getTextureToDisplay();
    if (pixmap != nullptr) {
    ui.labelImage->setPixmap(*pixmap);
    ui.labelImage->setFixedSize(pixmap->width(), pixmap->height());
    } else {
        ui.labelImage->clear();
        ui.labelImage->setFixedSize(0, 0);
    }
}

void EditTextureForm::refreshZoomDisplayValue() {
    ui.labelZoomValue->setText(fmt::format("{}%", ui.horizontalSliderZoom->value()).c_str());
}

void EditTextureForm::onPushButtonOK() {
    auto textureInfo = m_controller.createTextureDTOFromFields();

    if (!m_controller.validateTextureOperation(std::move(textureInfo))) {
        ErrorMessage::show(m_controller.getLastError());
        return;
    }
    accept();
}

void EditTextureForm::onLineEditNameTextChanged(const QString &text) {
    m_controller.setName(text.toStdString());
}

void EditTextureForm::onPushButtonOpenFilenameClick() {
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

void EditTextureForm::onSpinBoxTileWidthValueChanged(int value) {
    m_controller.setTileWidth(value);
    refreshTextureImage();
}

void EditTextureForm::onSpinBoxTileHeightValueChanged(int value) {
    m_controller.setTileHeight(value);
    refreshTextureImage();
}

void EditTextureForm::onHorizontalSliderZoomChanged(int value) {
    m_controller.setZoom(value);
    refreshZoomDisplayValue();
    refreshTextureImage();
}

void EditTextureForm::onPushButtonGridBlackClick() {
    m_controller.setGridColor(QColor::fromRgb(0, 0, 0));
    refreshTextureImage();
}

void EditTextureForm::onPushButtonGridWhiteClick() {
    m_controller.setGridColor(QColor::fromRgb(255, 255, 255));
    refreshTextureImage();
}
