#include "textureListComponent.hpp"
#include <QStyle>
#include <fmt/format.h>
#include "editTextureForm.hpp"
#include "texture.hpp"
#include "uiUtils.hpp"

using thewarrior::models::Texture;
using commoneditor::ui::UIUtils;

TextureListComponent::TextureListComponent(QWidget *parent,
        MainForm_GLComponent *glComponent)
    : QWidget(parent),
      ui(Ui::TextureListComponent()),
      m_glComponent(glComponent) {
      ui.setupUi(this);
      connectUIActions();
}

void TextureListComponent::connectUIActions() {
    connect(ui.pushButtonAddTexture,
            &QPushButton::clicked,
            this,
            &TextureListComponent::onPushButtonAddTextureClick);
    connect(ui.pushButtonEditTexture,
            &QPushButton::clicked,
            this,
            &TextureListComponent::onPushButtonEditTextureClick);
    connect(ui.pushButtonDeleteTexture,
            &QPushButton::clicked,
            this,
            &TextureListComponent::onPushButtonDeleteTextureClick);
    connect(ui.tableWidgetTextures,
            &QTableWidget::itemDoubleClicked,
            this,
            &TextureListComponent::onPushButtonEditTextureClick);
    tableWidgetTextureKeyWatcher.installOn(ui.tableWidgetTextures);
    connect(&tableWidgetTextureKeyWatcher,
            &QTableWidgetKeyPressWatcher::keyPressed,
            this,
            &TextureListComponent::onTableWidgetTexturesKeyPressEvent);
}

void TextureListComponent::refreshTextureList() {
    ui.tableWidgetTextures->model()->removeRows(0,
            ui.tableWidgetTextures->rowCount());
    int index {0};
    for (const auto &texture : m_glComponent->getTextures()) {
        ui.tableWidgetTextures->insertRow(index);
        ui.tableWidgetTextures->setItem(index,
                0,
                new QTableWidgetItem(texture.getName().c_str()));
        index++;
    }
}

std::optional<std::reference_wrapper<const Texture>> TextureListComponent::getSelectedTextureInTextureList() const {
    if (ui.tableWidgetTextures->selectionModel()->hasSelection()) {
        // Find the selected texture
        auto selectedItemName {
            ui.tableWidgetTextures->selectionModel()->selectedRows()[0].data().toString().toStdString()
        };
        return m_glComponent->getTextureByName(selectedItemName);
    } else {
        return std::nullopt;
    }
}

void TextureListComponent::onPushButtonAddTextureClick() {
    m_glComponent->stopAutoUpdate();
    auto alreadyUsedTextureNames { m_glComponent->getAlreadyUsedTextureNames() };
    EditTextureForm formEditTexture(this,
            m_glComponent->getResourcesPath(),
            nullptr,
           alreadyUsedTextureNames);
    UIUtils::centerToScreen(&formEditTexture);
    if (formEditTexture.exec() == QDialog::Accepted) {
        emit textureAdded(formEditTexture.getTextureInfo());
    }
    m_glComponent->startAutoUpdate();
}

void TextureListComponent::onPushButtonEditTextureClick() {
    m_glComponent->stopAutoUpdate();
    auto selectedTexture = getSelectedTextureInTextureList();
    if (selectedTexture.has_value()) {
        auto alreadyUsedTextureNames = m_glComponent->getAlreadyUsedTextureNames();
        // Remove the actual selected texture name
        auto iter = std::find(alreadyUsedTextureNames.begin(),
                alreadyUsedTextureNames.end(),
                selectedTexture->get().getName());
        if (iter != alreadyUsedTextureNames.end()) {
            alreadyUsedTextureNames.erase(iter);
        }
        EditTextureForm formEditTexture(this,
                m_glComponent->getResourcesPath(),
                &selectedTexture->get(),
                alreadyUsedTextureNames);
        if (formEditTexture.exec() == QDialog::Accepted) {
            emit textureUpdated(selectedTexture->get().getName(),
                    formEditTexture.getTextureInfo());
        }
    }
    m_glComponent->startAutoUpdate();
}

void TextureListComponent::onPushButtonDeleteTextureClick() {
    auto selectedTexture = getSelectedTextureInTextureList();
    if (selectedTexture.has_value()) {
        QMessageBox msgBox;
        msgBox.setText(fmt::format("Are you sure you want to delete the texture {0}?",
                    selectedTexture->get().getName()).c_str());
        msgBox.setWindowTitle("Confirmation");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Yes) {
            // Check if the texture is used in the map
            msgBox.setText(fmt::format("The texture {0} is used by some map tiles.\nAre you sure you want to proceed?",
                        selectedTexture->get().getName()).c_str());
            bool isUsed = m_glComponent->isTextureUsedInMap(selectedTexture->get().getName());
            if (!isUsed || msgBox.exec() == QMessageBox::Yes) {
                emit textureDeleted(selectedTexture->get().getName());
            }
        }
    }
}

void TextureListComponent::onTableWidgetTexturesKeyPressEvent(int key, int, int) {
    if (key == Qt::Key_Delete) {
        onPushButtonDeleteTextureClick();
    }
}
