#include <SDL2/SDL_timer.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <cstddef>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include "glBattleWindow.hpp"
#include "fadeLoopAnimation.hpp"
#include "shakingAnimation.hpp"
#include "valueChangeAnimation.hpp"
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
      m_namedObjects(std::map<std::string, GLObject>()),
      m_monsterHPBarWindow(std::vector<GLObject>()),
      m_monster(nullptr),
      m_monsterInitial(nullptr),
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

bool GLBattleWindow::initBattleShaders(const std::string &resourcesPath) {
    m_monsterHealthShaderProgram = std::make_shared<GLShaderProgram>(fmt::format("{0}/shaders/monster_330_vs.glsl", resourcesPath),
            fmt::format("{0}/shaders/monster_330_fs.glsl", resourcesPath));
    if (!m_monsterHealthShaderProgram->compileShaders()) {
        m_lastError = m_monsterHealthShaderProgram->getLastError();
        return false;
    }
    if (!m_monsterHealthShaderProgram->linkShaders({ "vertex" })) {
        m_lastError = m_monsterHealthShaderProgram->getLastError();
        return false;
    }
    return true;
}

void GLBattleWindow::reset() {
    m_battleLog = std::queue<std::string>();
    m_menuActionsPosition = 0;
    m_currentBattleAction = BattleAction::PlayerTurn;
    m_namedObjectsAnimations.clear();
    m_goldObtained = 0;
}

void GLBattleWindow::update() {
    if (m_namedObjectsAnimations.contains(MonsterHPBarObj)) {
        m_namedObjectsAnimations[MonsterHPBarObj]->process();
    }
    if (m_namedObjectsAnimations.contains(MonsterShaking)) {
        m_namedObjectsAnimations[MonsterShaking]->process();
    }
    if (m_currentBattleAction == BattleAction::PlayerTurn) {
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
            switch (m_currentBattleAction) {
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
                case BattleAction::PlayerGetReward:
                    playerObtainRewardWorkflow();
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
    m_monsterHPBarWindow.clear();
    // Equipments
    auto equipment = m_glPlayer->getEquipment();
    addWindowPanel({260.0F, 60.0F}, {280.0F, 280.0F}, 17);
    addWindowPanel({30.0F, 60.0F}, {190.0F, 150.0F}, 17);
    addWindowPanel({30.0F, 360.0F}, {190.0F, 180.0F}, 17);
    addWindowPanel({260.0F, 360.0F}, {510.0F, 200.0F}, 17);
    for (size_t i = 0; i < m_glTextActions.size(); i++) {
        auto actionElement = m_glTextActions.at(i);
        if (m_menuActionsPosition == i && m_currentBattleAction == BattleAction::PlayerTurn) {
            actionElement.color = GLColor::Green;
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
    generateBoxQuad(m_monsterHPBarWindow,
                    {335.0F, 325.0F},
                    {130.0F, 20.0F},
                    &m_windowGLTexture.texture,
                    8,
                    m_windowGLTexture.glTextureId, 16);
    generateQuad(battleObjects, {338.0F, 328.0F}, {124.0F, 14.0F}, &m_windowGLTexture.texture, 26, m_windowGLTexture.glTextureId);
    m_namedObjects[MoreTextObj] = battleObjects.at(0);
    m_namedObjects[MonsterObj] = battleObjects.at(1);
    m_namedObjects[MonsterHPBarObj] = battleObjects.at(2);
}

void GLBattleWindow::render() {
    GLPopupWindow::render();
    if (m_namedObjectsAnimations.contains(MoreTextObj)) {
        float moreTextTransparency = m_namedObjectsAnimations.contains(MoreTextObj) ?
            m_namedObjectsAnimations[MoreTextObj]->getValue() :
            0.0F;
        m_glFormService->drawQuad(m_namedObjects.at(MoreTextObj),
                                  m_namedObjects.at(MoreTextObj).textureGLId,
                                  moreTextTransparency);
    }
    float monsterTransparency = m_namedObjectsAnimations.contains(MonsterObj) ?
        m_namedObjectsAnimations[MonsterObj]->getValue() :
        0.0F;
    drawMonster(m_namedObjects.at(MonsterObj), m_namedObjects.at(MonsterObj).textureGLId, monsterTransparency);
    for (const auto &obj : m_monsterHPBarWindow) {
        m_glFormService->drawQuad(obj, m_windowGLTexture.glTextureId, monsterTransparency);
    }
    drawMonsterHPBar(m_namedObjects.at(MonsterHPBarObj), m_namedObjects.at(MonsterHPBarObj).textureGLId, monsterTransparency);
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
    m_monsterInitial = std::make_unique<Monster>(*monster);
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
    } else if (m_menuActionsPosition == 1) {  // Spell
    } else if (m_menuActionsPosition == 3) {  // Run
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
        m_currentBattleAction = action;
        m_actionStepStartTicks = SDL_GetTicks64();
        m_actionStepNextTicks = timeLength;
}

void GLBattleWindow::playerAttackWorkflow() {
    if (m_currentBattleAction == BattleAction::PlayerAttack) {
        //TODO: Check block, critical and miss
        //TODO Calculate the DPS
        int dps = 20;
        addBattleLog(fmt::format("You hit and HPs have been reduces by {0}!", dps).c_str());
        float oldMonsterHealthRatio = m_monster->getHealthRatio();
        m_monster->reduceHealth(dps);
        m_namedObjectsAnimations[MonsterShaking] = std::make_shared<ShakingAnimation>(25, 3);
        m_namedObjectsAnimations[MonsterHPBarObj] = std::make_shared<ValueChangeAnimation>(oldMonsterHealthRatio,
                m_monster->getHealthRatio(),
                0.075F);
        if (m_monster->isDead()) {
            m_namedObjectsAnimations[MonsterObj] = std::make_shared<ValueChangeAnimation>(1.0F, 0.1F, 0.05F);
            addBattleLog(fmt::format("You have defeated the {0}", m_monster->getName()).c_str());
            std::uniform_int_distribution<> distributionReward(m_monster->getGoldRewardRange().first,
                                                                        m_monster->getGoldRewardRange().second);
            m_goldObtained = distributionReward(RandomGenerator::instance());
            if (m_goldObtained > 0) {
                m_glPlayer->addGold(m_goldObtained);
                m_namedObjectsAnimations[MoreTextObj] = std::make_shared<FadeLoopAnimation>(0.1F, 1.0F, 0.01F);
            }
            startAction(BattleAction::PlayerWon, 500);
        } else {
            startAction(BattleAction::MonsterTurn, 500);
        }
    }
}

void GLBattleWindow::playerRunWorkflow() {
    if (m_currentBattleAction == BattleAction::PlayerTryToRun) {
        std::uniform_int_distribution<> distributionAttemptingToRun(1, 2);
        if (distributionAttemptingToRun(RandomGenerator::instance()) == 1) {
            startAction(BattleAction::PlayerRanAway, 750);
            addBattleLog("You ran away!");
        } else {
            m_currentBattleAction = BattleAction::PlayerTurn;
            m_menuActionsPosition = 0;
            addBattleLog("You were not able to run away! The battle continues...");
            startAction(BattleAction::MonsterTurn, 500);
        }
    } else if (m_currentBattleAction == BattleAction::PlayerRanAway) {
        m_battleCompleted();
    }
}

void GLBattleWindow::playerWonWorkflow() {
    m_namedObjectsAnimations[MoreTextObj]->process();
    m_namedObjectsAnimations[MonsterObj]->process();
    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        m_namedObjectsAnimations.erase(MoreTextObj);
        m_currentBattleAction = BattleAction::PlayerGetReward;
        addBattleLog(fmt::format("You obtain {0} gold!", m_goldObtained).c_str());
    }
}

void GLBattleWindow::playerObtainRewardWorkflow() {
    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        m_battleCompleted();
    }
}

void GLBattleWindow::monsterTurnWorkflow() {
    addBattleLog("Monster turn!");
    startAction(BattleAction::PlayerTurn, 750);
    generateGLElements();
}

void GLBattleWindow::monsterAttackWorkflow() {
}

void GLBattleWindow::drawMonster(const GLObject &glObject, GLuint textureGLIndex, float transparency) {
    m_monsterHealthShaderProgram->use();
    GLint uniformTransparency = glGetUniformLocation(m_monsterHealthShaderProgram->getShaderProgramID(), "transparency");
    glUniform1f(uniformTransparency, transparency);
    glUniform1f(glGetUniformLocation(m_monsterHealthShaderProgram->getShaderProgramID(), "widthScale"), 1.0F);
    glUniform1f(glGetUniformLocation(m_monsterHealthShaderProgram->getShaderProgramID(), "screenWidth"), m_screenSize.width());
    float translationX = m_namedObjectsAnimations.contains(MonsterShaking) &&
                   !m_namedObjectsAnimations.at(MonsterShaking)->isCompleted() ?
        m_namedObjectsAnimations.at(MonsterShaking)->getValue() :
        0.0F;
    glUniform2f(glGetUniformLocation(m_monsterHealthShaderProgram->getShaderProgramID(), "translation"), translationX, 0.0F);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureGLIndex);
    glBindVertexArray(glObject.vao);
    glBindBuffer(GL_ARRAY_BUFFER, glObject.vboPosition);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, glObject.vboColor);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, glObject.vboTexture);
    glEnableVertexAttribArray(2);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLBattleWindow::drawMonsterHPBar(const GLObject &glObject,
                                      GLuint textureGLIndex,
                                      float transparency) {
    m_monsterHealthShaderProgram->use();
    GLint uniformTransparency = glGetUniformLocation(m_monsterHealthShaderProgram->getShaderProgramID(), "transparency");
    glUniform1f(uniformTransparency, transparency);
    float health = m_namedObjectsAnimations.contains(MonsterHPBarObj) &&
                   !m_namedObjectsAnimations.at(MonsterHPBarObj)->isCompleted() ?
        m_namedObjectsAnimations.at(MonsterHPBarObj)->getValue() :
        m_monster->getHealthRatio();
    glUniform1f(glGetUniformLocation(m_monsterHealthShaderProgram->getShaderProgramID(), "widthScale"), health);
    glUniform1f(glGetUniformLocation(m_monsterHealthShaderProgram->getShaderProgramID(), "screenWidth"), m_screenSize.width());
    glUniform2f(glGetUniformLocation(m_monsterHealthShaderProgram->getShaderProgramID(), "translation"), 0.0F, 0.0F);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureGLIndex);
    glBindVertexArray(glObject.vao);
    glBindBuffer(GL_ARRAY_BUFFER, glObject.vboPosition);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, glObject.vboColor);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, glObject.vboTexture);
    glEnableVertexAttribArray(2);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

}  // namespace thewarrior::ui
