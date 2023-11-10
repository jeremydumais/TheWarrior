#include "editTextureForm.hpp"
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <fmt/format.h>
#include <algorithm>
#include <memory>
#include <utility>
#include "editTextureFormController.hpp"
#include "errorMessage.hpp"
#include "texture.hpp"
#include "textureDTO.hpp"

using commoneditor::ui::EditTextureFormController;
using commoneditor::ui::ErrorMessage;
using commoneditor::ui::TextureDTO;
using thewarrior::models::Texture;

EditTextureForm::EditTextureForm(QWidget *parent,
        const std::string &resourcesPath,
        const Texture * const texture,
        const std::vector<std::string> &alreadyUsedNames)
    : QDialog(parent),
    ui(Ui::editTextureFormClass()),
    m_controller(nullptr),
    m_isEditMode(texture != nullptr) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));

    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditTextureForm::onPushButtonOK);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditTextureForm::reject);
    connect(ui.lineEditName, &QLineEdit::textChanged, this, &EditTextureForm::onLineEditNameTextChanged);
    connect(ui.pushButtonOpenFilename, &QPushButton::clicked, this, &EditTextureForm::onPushButtonOpenFilenameClick);
    connect(ui.spinBoxTileWidth, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &EditTextureForm::onSpinBoxTileWidthValueChanged);
    connect(ui.spinBoxTileHeight, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &EditTextureForm::onSpinBoxTileHeightValueChanged);
    connect(ui.horizontalSliderZoom, &QSlider::valueChanged, this, &EditTextureForm::onHorizontalSliderZoomChanged);
    connect(ui.pushButtonGridBlack, &QPushButton::clicked, this, &EditTextureForm::onPushButtonGridBlackClick);
    connect(ui.pushButtonGridWhite, &QPushButton::clicked, this, &EditTextureForm::onPushButtonGridWhiteClick);

    std::unique_ptr<TextureDTO> originalTexture = nullptr;
    if (m_isEditMode) {
        originalTexture = std::make_unique<TextureDTO>();
        originalTexture->name = texture->getName();
        originalTexture->filename = texture->getFilename();
        originalTexture->width = texture->getWidth();
        originalTexture->height = texture->getHeight();
        originalTexture->tileWidth = texture->getTileWidth();
        originalTexture->tileHeight = texture->getTileHeight();
    }
    m_controller = std::make_unique<EditTextureFormController>(std::move(originalTexture),
            alreadyUsedNames,
            resourcesPath);

    if (!m_isEditMode) {
        this->setWindowTitle("Add texture");
    } else {
        this->setWindowTitle("Edit texture");
        loadExistingItemToForm();
    }
    refreshZoomDisplayValue();
}

void EditTextureForm::loadExistingItemToForm() {
    const auto &texture = m_controller->getOriginalTexture();
    ui.lineEditName->setText(texture.name.c_str());
    ui.lineEditFilename->setText(texture.filename.c_str());
    ui.labelImageWidth->setText(QString::number(texture.width));
    ui.labelImageHeight->setText(QString::number(texture.height));
    ui.spinBoxTileWidth->setValue(texture.tileWidth);
    ui.spinBoxTileHeight->setValue(texture.tileHeight);
    m_controller->loadTextureImage(texture.filename);
    refreshTextureImage();
}

void EditTextureForm::refreshTextureImage() {
    const auto pixmap = m_controller->getTextureToDisplay();
    if (pixmap != nullptr) {
    ui.labelImage->setPixmap(*pixmap);
    ui.labelImage->setFixedSize(pixmap->width(), pixmap->height());
    } else {
        ui.labelImage->clear();
        ui.labelImage->setFixedSize(0, 0);
    }
}

TextureDTO EditTextureForm::getTextureInfo() const {
    return *m_controller->createTextureDTOFromFields().get();
}

void EditTextureForm::refreshZoomDisplayValue() {
    ui.labelZoomValue->setText(fmt::format("{}%", ui.horizontalSliderZoom->value()).c_str());
}

void EditTextureForm::onPushButtonOK() {
    auto textureInfo = m_controller->createTextureDTOFromFields();

    if (!m_controller->validateTextureOperation(std::move(textureInfo))) {
        ErrorMessage::show(m_controller->getLastError());
        return;
    }
    accept();
}

void EditTextureForm::showErrorMessage(const std::string &message) const {
    QMessageBox msgBox;
    msgBox.setText(message.c_str());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle("Error");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void EditTextureForm::onLineEditNameTextChanged(const QString &text) {
    m_controller->setName(text.toStdString());
}

void EditTextureForm::onPushButtonOpenFilenameClick() {
    const char *resourcesPath = m_controller->getResourcesPath().c_str();
    QString fullFilePath { QFileDialog::getOpenFileName(this,
            tr("Open Texture"),
            resourcesPath,
            tr("Images (*.png)")) };
    QFileInfo fileInfo(fullFilePath);
    std::string filename { fileInfo.fileName().toStdString() };
    ui.lineEditFilename->setText(filename.c_str());
    if (auto result = m_controller->loadTextureImage(filename); result.Success) {
        ui.labelImageWidth->setText(QString::number(result.TextureWidth));
        ui.labelImageHeight->setText(QString::number(result.TextureHeight));
        refreshTextureImage();
    } else {
        ErrorMessage::show(m_controller->getLastError());
        ui.lineEditFilename->clear();
    }
}

void EditTextureForm::onSpinBoxTileWidthValueChanged(int value) {
    m_controller->setTileWidth(value);
    refreshTextureImage();
}

void EditTextureForm::onSpinBoxTileHeightValueChanged(int value) {
    m_controller->setTileHeight(value);
    refreshTextureImage();
}

void EditTextureForm::onHorizontalSliderZoomChanged(int value) {
    m_controller->setZoom(value);
    refreshZoomDisplayValue();
    refreshTextureImage();
}

void EditTextureForm::onPushButtonGridBlackClick() {
    m_controller->setGridColor(QColor::fromRgb(0, 0, 0));
    refreshTextureImage();
}

void EditTextureForm::onPushButtonGridWhiteClick() {
    m_controller->setGridColor(QColor::fromRgb(255, 255, 255));
    refreshTextureImage();
}
