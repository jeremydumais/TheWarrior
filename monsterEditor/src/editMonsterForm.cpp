#include <fmt/format.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qstring.h>
#include <qtimer.h>
#include <qwidget.h>
#include <memory>
#include <string>
#include <utility>
#include <QFileInfo>
#include "editMonsterForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"
#include "textureUtils.hpp"
#include <boost/algorithm/string.hpp>

using namespace commoneditor::ui;
using namespace monstereditor::controllers;
using namespace thewarrior::models;

struct TextureSelectionInfo {
    std::string textureName;
    int textureIndex;
};

std::optional<TextureSelectionInfo> showTexturePicker(QWidget *parent,
        const std::string &resourcesPath,
        const TextureSelectionInfo &info,
        const TextureContainer &textureContainer);

EditMonsterForm::EditMonsterForm(QWidget *parent,
        const std::string &resourcesPath,
        std::shared_ptr<MonsterStore> monsterStore,
        std::optional<std::string> monsterIdToEdit)
    : QDialog(parent),
    ui(Ui::editMonsterFormClass()),
    m_controller(monsterStore),
    m_resourcesPath(resourcesPath),
    m_monsterIdToEdit(monsterIdToEdit) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MonsterEditor Icon.png"));
    window()->layout()->setSizeConstraint(QLayout::SetFixedSize);

    connectUIActions();
    if (m_monsterIdToEdit.has_value()) {
        this->setWindowTitle("Edit monster");
        if (!loadExistingMonsterToForm()) {
            QTimer::singleShot(0, this, SLOT(close()));
        }
    }
}

void EditMonsterForm::connectUIActions() {
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditMonsterForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditMonsterForm::onPushButtonOKClick);
    connect(ui.pushButtonTexturePicker, &QPushButton::clicked, this, &EditMonsterForm::onPushButtonTexturePickerClick);
    connect(ui.pushButtonOpenMusicFile, &QPushButton::clicked, this, &EditMonsterForm::onPushButtonOpenMusicFileClick);
    connect(ui.pushButtonClearMusic, &QPushButton::clicked, this, &EditMonsterForm::onPushButtonClearMusicClick);
}

bool EditMonsterForm::loadExistingMonsterToForm() {
    auto existingMonster = m_controller.getMonster(*m_monsterIdToEdit);
    if (existingMonster != nullptr) {
        ui.lineEditId->setText(existingMonster->id.c_str());
        ui.lineEditName->setText(existingMonster->name.c_str());
        ui.lineEditTextureName->setText(existingMonster->textureName.c_str());
        ui.spinBoxTextureIndex->setValue(existingMonster->textureIndex);
        ui.spinBoxHealthMin->setValue(existingMonster->healthRange.first);
        ui.spinBoxHealthMax->setValue(existingMonster->healthRange.second);
        ui.doubleSpinBoxAttack->setValue(static_cast<double>(existingMonster->attack));
        ui.doubleSpinBoxDefense->setValue(static_cast<double>(existingMonster->defense));
        ui.spinBoxGoldMin->setValue(existingMonster->gold.first);
        ui.spinBoxGoldMax->setValue(existingMonster->gold.second);
        ui.spinBoxExperienceMin->setValue(existingMonster->experience.first);
        ui.spinBoxExperienceMax->setValue(existingMonster->experience.second);
        if (existingMonster->type == MonsterType::Boss) {
            ui.radioButtonTypeBoss->setChecked(true);
        } else {
            ui.radioButtonTypeRegular->setChecked(true);
        }
        ui.lineEditMusicFilename->setText(existingMonster->musicFilename.c_str());
        refreshSelectedTexture();
    } else {
        ErrorMessage::show("Unable to load the selected monster");
        return false;
    }
    return true;
}

void EditMonsterForm::onPushButtonCancelClick() {
    reject();
}

void EditMonsterForm::onPushButtonOKClick() {
    if (!ui.radioButtonTypeRegular->isChecked() && !ui.radioButtonTypeBoss->isChecked()) {
        ErrorMessage::show("The monster type is required.");
        return;
    }

    auto monsterInfo = std::make_unique<MonsterDTO>();
    monsterInfo->id = ui.lineEditId->text().toStdString();
    monsterInfo->name = ui.lineEditName->text().toStdString();
    monsterInfo->textureName = ui.lineEditTextureName->text().toStdString();
    monsterInfo->textureIndex = ui.spinBoxTextureIndex->value();
    monsterInfo->healthRange.first = ui.spinBoxHealthMin->value();
    monsterInfo->healthRange.second = ui.spinBoxHealthMax->value();
    monsterInfo->maxHealth = ui.spinBoxHealthMax->value();
    monsterInfo->attack = static_cast<float>(ui.doubleSpinBoxAttack->value());
    monsterInfo->defense = static_cast<float>(ui.doubleSpinBoxDefense->value());
    monsterInfo->gold.first = ui.spinBoxGoldMin->value();
    monsterInfo->gold.second = ui.spinBoxGoldMax->value();
    monsterInfo->experience.first = ui.spinBoxExperienceMin->value();
    monsterInfo->experience.second = ui.spinBoxExperienceMax->value();
    MonsterType type = MonsterType::Regular;;
    if (ui.radioButtonTypeBoss->isChecked()) {
        type = MonsterType::Boss;
    }
    monsterInfo->type = type;
    monsterInfo->musicFilename = ui.lineEditMusicFilename->text().trimmed().toStdString();

    if (!m_monsterIdToEdit.has_value()) {
        if (!m_controller.addMonster(std::move(monsterInfo))) {
            ErrorMessage::show(m_controller.getLastError());
            return;
        }
    } else {
        if (!m_controller.updateMonster(std::move(monsterInfo), *m_monsterIdToEdit)) {
            ErrorMessage::show(m_controller.getLastError());
            return;
        }
    }
    accept();
}

void EditMonsterForm::onPushButtonTexturePickerClick() {
    auto result = showTexturePicker(this,
            m_resourcesPath,
            { ui.lineEditTextureName->text().toStdString(),
            ui.spinBoxTextureIndex->value() },
            m_controller.getTextureContainer());
    if (result.has_value()) {
        ui.lineEditTextureName->setText(result->textureName.c_str());
        ui.spinBoxTextureIndex->setValue(result->textureIndex);
        refreshSelectedTexture();
    }
}

std::optional<TextureSelectionInfo> showTexturePicker(QWidget *parent,
        const std::string &resourcesPath,
        const TextureSelectionInfo &info,
        const TextureContainer &textureContainer) {
    TexturePickerForm texturePickerForm(parent,
            resourcesPath,
            textureContainer);
    auto selectedTexture = info.textureName;
    if (!boost::trim_copy(selectedTexture).empty()) {
        texturePickerForm.setCurrentSelection(selectedTexture, info.textureIndex);
    }
    if (texturePickerForm.exec() == QDialog::Accepted) {
        const auto &result = texturePickerForm.getResult();
        return TextureSelectionInfo {
            .textureName = result.textureName,
            .textureIndex = result.textureIndex
        };
    }
    return std::nullopt;
}

void EditMonsterForm::refreshSelectedTexture() {
    ui.labelIcon->clear();
    if (!ui.lineEditTextureName->text().isEmpty()) {
        const auto &textureContainer = m_controller.getTextureContainer();
        auto texture = textureContainer.getTextureByName(ui.lineEditTextureName->text().toStdString());
        if (texture.has_value()) {
            auto completeTexturePath = fmt::format("{0}/textures/{1}", m_resourcesPath, texture->get().getFilename());
            QPixmap pixmap(QString(completeTexturePath.c_str()));
            auto iconPixmap = TextureUtils::getTextureTileImageFromTexture(&pixmap,
                    ui.spinBoxTextureIndex->value(),
                    texture.value());
            iconPixmap = iconPixmap.scaled(ui.labelIcon->maximumSize(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation);
            ui.labelIcon->setPixmap(iconPixmap);
        }
    }
}

void EditMonsterForm::onPushButtonOpenMusicFileClick() {
    QString fullFilePath { QFileDialog::getOpenFileName(this,
            tr("Open Music file"),
            m_resourcesPath.c_str(),
            tr("Music File (*.mp3)")) };
    QFileInfo fileInfo(fullFilePath);
    std::string filename { fileInfo.fileName().toStdString() };
    ui.lineEditMusicFilename->setText(filename.c_str());
}

void EditMonsterForm::onPushButtonClearMusicClick() {
    ui.lineEditMusicFilename->clear();
}
