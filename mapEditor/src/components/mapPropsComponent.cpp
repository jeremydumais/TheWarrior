#include <fmt/format.h>
#include <qstring.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <QImageReader>
#include <stdexcept>
#include <string>
#include "mapPropsComponent.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"
#include "textureContainer.hpp"

using commoneditor::ui::ErrorMessage;
using thewarrior::models::TextureContainer;
using thewarrior::models::TextureInfo;

MapPropsComponent::MapPropsComponent(QWidget *parent,
        MainForm_GLComponent *glComponent)
    : QWidget(parent),
      ui(Ui::MapPropsComponent()),
      m_glComponent(glComponent) {
      ui.setupUi(this);
      connectUIActions();
}

void MapPropsComponent::connectUIActions() {
    connect(ui.pushButtonApplySizeChange, &QPushButton::clicked, this, &MapPropsComponent::onPushButtonApplySizeChangeClick);
    connect(ui.pushButtonOpenMusicFile, &QPushButton::clicked, this, &MapPropsComponent::onPushButtonOpenMusicFileClick);
    connect(ui.pushButtonClearMusic, &QPushButton::clicked, this, &MapPropsComponent::onPushButtonClearMusicClick);
    connect(ui.pushButtonOpenBattleLandscapeTexturePicker, &QPushButton::clicked, this, &MapPropsComponent::onPushButtonOpenBattleLandscapeTexturePickerClick);
    connect(ui.pushButtonClearBattleLandscapeTextureIndex, &QPushButton::clicked, this, &MapPropsComponent::onPushButtonClearBattleLandscapeTextureIndexClick);
}

void MapPropsComponent::reset() {
    ui.lineEditMapWidth->setText(std::to_string(m_glComponent->getMapWidth()).c_str());
    ui.lineEditMapHeight->setText(std::to_string(m_glComponent->getMapHeight()).c_str());
    ui.spinBoxMapSizeLeft->setValue(0);
    ui.spinBoxMapSizeTop->setValue(0);
    ui.spinBoxMapSizeRight->setValue(0);
    ui.spinBoxMapSizeBottom->setValue(0);
    ui.lineEditMusicFilename->setText(m_glComponent->getMapMusicFilename().c_str());
    setBattleLandscapeTextureIndex(m_glComponent->getBattleLandscapeTextureIndex());
}

void MapPropsComponent::refresh() {
    ui.lineEditMapWidth->setText(std::to_string(m_glComponent->getMapWidth()).c_str());
    ui.lineEditMapHeight->setText(std::to_string(m_glComponent->getMapHeight()).c_str());
    ui.lineEditMusicFilename->setText(m_glComponent->getMapMusicFilename().c_str());
    setBattleLandscapeTextureIndex(m_glComponent->getBattleLandscapeTextureIndex());
}

void MapPropsComponent::setResourcesPath(const std::string &resourcesPath) {
    m_resourcesPath = resourcesPath;
}

void MapPropsComponent::setBattleLandscapeTextureIndex(int textureIndex) {
    ui.lineEditBattleLandscapeTextureIndex->setText(textureIndex == -1 ? "" : QString::number(textureIndex));
}

void MapPropsComponent::onPushButtonApplySizeChangeClick() {
    int offsetLeft { ui.spinBoxMapSizeLeft->value() };
    int offsetTop { ui.spinBoxMapSizeTop->value() };

    int offsetRight { ui.spinBoxMapSizeRight->value() };
    int offsetBottom { ui.spinBoxMapSizeBottom->value() };
    if (offsetLeft < 0 ||
            offsetTop < 0 ||
            offsetRight < 0 ||
            offsetBottom < 0) {
        // Check if there's tiles that are already assigned in the ones we will remove
        if (m_glComponent->isShrinkMapImpactAssignedTiles(offsetLeft,
                    offsetTop,
                    offsetRight,
                    offsetBottom)) {
            ErrorMessage::show("Cannot resize the map because some tile are\n"
                    "already assigned in the ones you try to remove.");
            return;
        }
    }
    emit onBeforeApplyChange();
    // Apply new size
    try {
        m_glComponent->resizeMap(offsetLeft,
                offsetTop,
                offsetRight,
                offsetBottom);
        reset();
    } catch(std::invalid_argument &err) {
        ErrorMessage::show(err.what());
    }
}

void MapPropsComponent::onPushButtonOpenMusicFileClick() {
    QString fullFilePath { QFileDialog::getOpenFileName(this,
            tr("Open Music file"),
            m_resourcesPath.c_str(),
            tr("Music File (*.mp3)")) };
    QFileInfo fileInfo(fullFilePath);
    std::string filename { fileInfo.fileName().toStdString() };
    ui.lineEditMusicFilename->setText(filename.c_str());   
    emit onMusicChanged(filename);
}

void MapPropsComponent::onPushButtonClearMusicClick() {
    ui.lineEditMusicFilename->clear();
    emit onMusicChanged("");
}
 
void MapPropsComponent::onPushButtonOpenBattleLandscapeTexturePickerClick() {
    // Prepare the Texture container
    TextureContainer textureContainer;
    const std::string filename = "battle_landscape.png";
    QImageReader reader(QString::fromStdString(fmt::format("{}/textures/{}", m_resourcesPath, filename)));
    const QSize imageSize = reader.size();
    if (!imageSize.isValid()) {
        ErrorMessage::show("Cannot read battle landscape image dimensions.");
        return;
    }
    if (!textureContainer.addTexture(TextureInfo {
        .name = "Battle landscapes",
        .filename = filename,
        .width = imageSize.width(),
        .height = imageSize.height(),
        .tileWidth = 512,
        .tileHeight = 512
    })) {
        ErrorMessage::show(textureContainer.getLastError());
        return;
    }
    TexturePickerForm pickerForm(this, m_resourcesPath, textureContainer);
    if (pickerForm.exec() == QDialog::Accepted) {
        int resultingTextureIndex = pickerForm.getResult().textureIndex;
        ui.lineEditBattleLandscapeTextureIndex->setText(QString::number(resultingTextureIndex));
        emit onBattleLandscapeTextureIndexChanged(resultingTextureIndex);
    }   
}

void MapPropsComponent::onPushButtonClearBattleLandscapeTextureIndexClick() {
    ui.lineEditBattleLandscapeTextureIndex->clear();
    emit onBattleLandscapeTextureIndexChanged(-1);
}
