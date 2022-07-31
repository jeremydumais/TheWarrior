#include "editMonsterForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"
#include "textureUtils.hpp"
#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <qdialog.h>
#include <qtimer.h>
#include <qwidget.h>

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
      m_monsterIdToEdit(monsterIdToEdit)
{
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

void EditMonsterForm::connectUIActions() 
{
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditMonsterForm::onPushButtonCancelClick);
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditMonsterForm::onPushButtonOKClick);
	connect(ui.pushButtonTexturePicker, &QPushButton::clicked, this, &EditMonsterForm::onPushButtonTexturePickerClick);
}

bool EditMonsterForm::loadExistingMonsterToForm()
{
	auto existingMonster = m_controller.getMonster(*m_monsterIdToEdit);
	if (existingMonster != nullptr) {
		ui.lineEditId->setText(existingMonster->id.c_str());
		ui.lineEditName->setText(existingMonster->name.c_str());
		ui.lineEditTextureName->setText(existingMonster->textureName.c_str());
		ui.spinBoxTextureIndex->setValue(existingMonster->textureIndex);
        ui.spinBoxHealth->setValue(existingMonster->health);
        ui.doubleSpinBoxAttack->setValue(static_cast<double>(existingMonster->attack));
        ui.doubleSpinBoxDefense->setValue(static_cast<double>(existingMonster->defense));
        ui.spinBoxGoldMin->setValue(existingMonster->goldMinimum);
        ui.spinBoxGoldMax->setValue(existingMonster->goldMaximum);
        refreshSelectedTexture();
	}
	else {
		ErrorMessage::show("Unable to load the selected monster");
		return false;
	}
	return true;
}

void EditMonsterForm::onPushButtonCancelClick()
{
	reject();
}

void EditMonsterForm::onPushButtonOKClick()
{
	auto monsterInfo = std::make_unique<MonsterDTO>();
	monsterInfo->id = ui.lineEditId->text().toStdString();
	monsterInfo->name = ui.lineEditName->text().toStdString();
	monsterInfo->textureName = ui.lineEditTextureName->text().toStdString();
	monsterInfo->textureIndex = ui.spinBoxTextureIndex->value();
    monsterInfo->health = ui.spinBoxHealth->value();
    monsterInfo->attack = static_cast<float>(ui.doubleSpinBoxAttack->value());
    monsterInfo->defense = static_cast<float>(ui.doubleSpinBoxDefense->value());
    monsterInfo->goldMinimum = ui.spinBoxGoldMin->value();
    monsterInfo->goldMaximum = ui.spinBoxGoldMax->value();

    if (!m_monsterIdToEdit.has_value()) {
		if (!m_controller.addMonster(std::move(monsterInfo))) {
			ErrorMessage::show(m_controller.getLastError());
			return;
		}
	} 
    else {
		if (!m_controller.updateMonster(std::move(monsterInfo), *m_monsterIdToEdit)) {
			ErrorMessage::show(m_controller.getLastError());
			return;
		}
	}
	accept();
}

void EditMonsterForm::onPushButtonTexturePickerClick()
{
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
                                                      const TextureContainer &textureContainer)
{
    TexturePickerForm texturePickerForm(parent, 
										resourcesPath,
										textureContainer);
	auto selectedTexture = info.textureName;
	if (!boost::trim_copy(selectedTexture).empty()) {
		texturePickerForm.setCurrentSelection(selectedTexture,
											  info.textureIndex);
	}
	if (texturePickerForm.exec() == QDialog::Accepted) {
		const auto &result = texturePickerForm.getResult();
        return TextureSelectionInfo { 
            result.textureName,
            result.textureIndex 
        };
	}
    return std::nullopt;
}

void EditMonsterForm::refreshSelectedTexture()
{
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
