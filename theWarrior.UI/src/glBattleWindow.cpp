#include <SDL2/SDL_timer.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include "glBattleWindow.hpp"
#include "glColor.hpp"
#include "glObjectService.hpp"
#include "monsterStore.hpp"
#include "randomUtils.hpp"

using namespace thewarrior::models;
using namespace thewarrior::utils;

namespace thewarrior::ui {

GLBattleWindow::GLBattleWindow()
    : GLPopupWindow({ 800.0F, 600.0F }),
      m_slotsGLTexture({ Texture(TextureInfo{ "battleLandscape", "battle_landscape.png", 1536, 512, 512, 512 }), 0 }),
      m_battleObjects(std::map<std::string, GLObject>()),
      m_monster(nullptr),
      m_glTextActions(std::vector<GLTextObject>()),
      m_menuActionsPosition(0),
      m_battleLog(std::queue<std::string>()) {
}

void GLBattleWindow::initialize(const std::string &resourcePath,
                                   std::shared_ptr<GLPlayer> glPlayer,
                                   std::shared_ptr<GLTextService> textService,
                                   std::shared_ptr<MonsterStore> monsterStore,
                                   const std::map<std::string, unsigned int> *texturesGLMonsterStore,
                                   std::shared_ptr<InputDevicesState> inputDevicesState) {
    GLPopupWindow::initialize("Battle", resourcePath, textService);
    m_glPlayer = glPlayer;
    m_monsterStore = monsterStore;
    m_texturesGLMonsterStore = texturesGLMonsterStore;
    m_inputDevicesState = inputDevicesState;
    m_textureService.loadTexture(m_slotsGLTexture);
    m_glTextActions.push_back({ "Fight", { 1.0F, 400.0F }, 0.6F });
    m_glTextActions.push_back({ "Spell", { 1.0F, 440.0F }, 0.6F });
    m_glTextActions.push_back({ "Item", { 1.0F, 480.0F }, 0.6F });
    m_glTextActions.push_back({ "Run", { 1.0F, 520.0F }, 0.6F });
}

void GLBattleWindow::reset() {
    m_battleLog = std::queue<std::string>();
    m_menuActionsPosition = 0;
    m_battleAction = BattleAction::PlayerTurn;
    monsterAlphaValue = 1.0F;
}

void GLBattleWindow::update() {
    if (m_battleAction == BattleAction::PlayerTurn) {
        const Uint64 MS_BETWEEN_SELECTION_CHANGE = 110;
        auto inputUpTicks = m_inputDevicesState->getUpPressedTicks();
        if (m_inputDevicesState->getUpPressed() &&
            inputUpTicks.has_value() &&
            (inputUpTicks.value() - m_lastMoveUpTicks) > MS_BETWEEN_SELECTION_CHANGE) {
            moveUpPressed();
            m_lastMoveUpTicks = inputUpTicks.value();
            return;
        } else if (!m_inputDevicesState->getUpPressed()) {
            m_lastMoveUpTicks = 0;
        }

        auto inputDownTicks = m_inputDevicesState->getDownPressedTicks();
        if (m_inputDevicesState->getDownPressed() &&
            inputDownTicks.has_value() &&
            (inputDownTicks.value() - m_lastMoveDownTicks) > MS_BETWEEN_SELECTION_CHANGE) {
            moveDownPressed();
            m_lastMoveDownTicks = inputDownTicks.value();
            return;
        } else if (!m_inputDevicesState->getDownPressed()) {
            m_lastMoveDownTicks = 0;
        }

        if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
            actionButtonPressed();
        }
    } else {
        if (SDL_GetTicks64() - m_actionStepStartTicks > m_actionStepNextTicks) {
            switch (m_battleAction) {
                case BattleAction::PlayerTurn:
                    break;
                case BattleAction::PlayerAttack:
                    playerAttackWorkflow();
                    break;
                case BattleAction::PlayerRanAway:
                case BattleAction::PlayerTryToRun:
                    playerRunWorkflow();
                    break;
                case BattleAction::PlayerWon:
                    playerWonWorkflow();
                    break;
                case BattleAction::MonsterTurn:
                    monsterTurnWorkflow();
                    break;
                case BattleAction::MonsterAttack:
                    monsterAttackWorkflow();
                    break;
                default:
                    break;
            }
        }
    }
}

void GLBattleWindow::generateGLElements() {
    GLPopupWindow::generateGLElements();
    //m_battleObjects.clear();
    // Equipments
    auto equipment = m_glPlayer->getEquipment();
    addWindowPanel({260.0F, 60.0F}, {280.0F, 280.0F}, 17);
    addWindowPanel({30.0F, 60.0F}, {190.0F, 150.0F}, 17);
    addWindowPanel({30.0F, 360.0F}, {190.0F, 180.0F}, 17);
    addWindowPanel({260.0F, 360.0F}, {510.0F, 200.0F}, 17);
    for (size_t i = 0; i < m_glTextActions.size(); i++) {
        auto actionElement = m_glTextActions.at(i);
        if (m_menuActionsPosition == i) {
            actionElement.color = GLColor::Gray;
        }
        addXCenteredTextObject(actionElement, 30.0F, 190.0F);
    }
    addXCenteredTextObject({"Warrior", {1.0F, 100.0F}, 0.5F}, 30.0F, 190.0F);
    addXCenteredTextObject({"HP 20/320", {1.0F, 140.0F}, 0.5F}, 30.0F, 190.0F);
    addXCenteredTextObject({"MP 12/120", {1.0F, 180.0F}, 0.5F}, 30.0F, 190.0F);
    auto battleLogCopy = m_battleLog;
    for (size_t i = 0; i < m_battleLog.size(); i++) {
        size_t colorIndex =  (m_battleLog.size() - 1) - i;
        GLColor color = GLColor::Gray;
        if (colorIndex == 0) {
            color = GLColor::White;
        } else if (colorIndex == 1 || colorIndex == 2) {
            color = GLColor::LightGray;
        }
        addXCenteredTextObject({
                battleLogCopy.front(), {
                    1.0F, 395.0F + (static_cast<float>(i) * 30.0F) + ((5.0F - static_cast<float>(m_battleLog.size())) * 30.0F)},
                    0.38F,
                    color
                },
                260.0F,
                510.0F);
        battleLogCopy.pop();
    }
    std::vector<GLObject> battleObjects = {};
    // More text icon
    m_glFormService->generateQuad(battleObjects,
                                  { m_windowLocation.x() + 499.0F,
                                    m_windowLocation.y() + 520.0F },
                                  { 32.0F, 32.0F}, &m_windowGLTexture.texture, 25, m_windowGLTexture.glTextureId);
    // Map landscape
    generateQuad(m_glObjects, {270.0F, 70.0F}, {260.0F, 260.0F}, &m_slotsGLTexture.texture, 0, m_slotsGLTexture.glTextureId);
    // Monster
    auto textureMonster = &m_monsterStore->getTextureContainer().getTextureByName(m_monster->getTextureName()).value().get();
    generateQuad(battleObjects,
                    {325, 175},
                    {150, 150},
                    textureMonster,
                    m_monster->getTextureIndex(),
                    m_texturesGLMonsterStore->at(m_monster->getTextureName()));
    m_battleObjects["moreText"] = battleObjects.at(0);
    m_battleObjects["monster"] = battleObjects.at(1);
}

void GLBattleWindow::render() {
    GLPopupWindow::render();
    if (m_battleAction == BattleAction::PlayerWon) {
        m_glFormService->drawQuad(m_battleObjects.at("moreText"), m_battleObjects.at("moreText").textureGLId, moreTextIconAlphaValue);
    }
    m_glFormService->drawQuad(m_battleObjects.at("monster"), m_battleObjects.at("monster").textureGLId, monsterAlphaValue);
}

void GLBattleWindow::gameWindowSizeChanged(const Size<> &size) {
    GLPopupWindow::gameWindowSizeChanged(size);
    generateGLElements();
}

void GLBattleWindow::prepareWindow(const std::string &id) {
    reset();
    auto monster = m_monsterStore->findMonster(id);
    if (!monster) {
        throw std::runtime_error(fmt::format("Unable to found the monster {0}", id));
    }
    m_monster = std::make_unique<Monster>(*monster);
    setTitle(m_monster->getName());
    addBattleLog(fmt::format("You encountered a {}!", m_monster->getName()).c_str());
    generateGLElements();
}

void GLBattleWindow::moveUpPressed() {
    if (m_menuActionsPosition > 0) {
        m_menuActionsPosition--;
        generateGLElements();
    }
}

void GLBattleWindow::moveDownPressed() {
    if (m_menuActionsPosition + 1 < m_glTextActions.size()) {
        m_menuActionsPosition++;
        generateGLElements();
    }
}

void GLBattleWindow::actionButtonPressed() {
    if (m_menuActionsPosition == 0) {  // Attack
        addBattleLog(fmt::format("You attack the {0}...", m_monster->getName()).c_str());
        startAction(BattleAction::PlayerAttack, 750);
    }
    if (m_menuActionsPosition == 3) {  // Run
        addBattleLog("Attempting to run away...");
        startAction(BattleAction::PlayerTryToRun, 750);
    }
}

void GLBattleWindow::addBattleLog(const std::string &log) {
    if (m_battleLog.size() == 5) {
        m_battleLog.pop();
    }
    m_battleLog.push(log);
    generateGLElements();
}

void GLBattleWindow::startAction(BattleAction action, Uint64 timeLength) {
        m_battleAction = action;
        m_actionStepStartTicks = SDL_GetTicks64();
        m_actionStepNextTicks = timeLength;
}

void GLBattleWindow::playerAttackWorkflow() {
    if (m_battleAction == BattleAction::PlayerAttack) {
        //TODO: Check block and miss
        //TODO Calculate the DPS
        int dps = 9;
        addBattleLog(fmt::format("You hit and HPs have been reduces by {0}!", dps).c_str());
        m_monster->setHealth(m_monster->getHealth() - dps);
        std::cout << m_monster->getHealth() << std::endl;
        if (m_monster->getHealth() <= 0) {
            startAction(BattleAction::PlayerWon, 500);
            addBattleLog(fmt::format("You have defeated the {0}", m_monster->getName()).c_str());
            //TODO: start the animation for the monster to disapear
        } else {
            startAction(BattleAction::MonsterTurn, 500);
        }
    }
}

void GLBattleWindow::playerRunWorkflow() {
    if (m_battleAction == BattleAction::PlayerTryToRun) {
        std::uniform_int_distribution<> distributionAttemptingToRun(1, 2);
        if (distributionAttemptingToRun(RandomGenerator::instance()) == 1) {
            startAction(BattleAction::PlayerRanAway, 750);
            addBattleLog("You ran away!");
        } else {
            m_battleAction = BattleAction::PlayerTurn;
            m_menuActionsPosition = 0;
            addBattleLog("You were not able to run away! The battle continues...");
            startAction(BattleAction::MonsterTurn, 500);
        }
    } else if (m_battleAction == BattleAction::PlayerRanAway) {
        m_battleCompleted();
    }
}

void GLBattleWindow::playerWonWorkflow() {
    if (moreTextIconAlphaIncrease)
        moreTextIconAlphaValue += 0.01F;
    else
        moreTextIconAlphaValue -= 0.01F;
    if (moreTextIconAlphaValue >= 1.0F) {
        moreTextIconAlphaIncrease = false;
    } else if (moreTextIconAlphaValue <= 0.1F) {
        moreTextIconAlphaIncrease = true;
    }
    if (monsterAlphaValue > 0.05F) {
        monsterAlphaValue -= 0.05F;
    }
    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        m_battleCompleted();
    }
}

void GLBattleWindow::monsterTurnWorkflow() {
    addBattleLog("Monster turn!");
    startAction(BattleAction::PlayerTurn, 750);
}

void GLBattleWindow::monsterAttackWorkflow() {
}

}  // namespace thewarrior::ui
