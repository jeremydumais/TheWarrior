#include <SDL2/SDL_timer.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <random>
#include <stdexcept>
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
    // Sounds
    m_attackSound = Mix_LoadWAV(fmt::format("{0}/sounds/attack.wav", m_resourcesPath).c_str());
    if (m_attackSound == nullptr) {
        std::cerr << fmt::format("Mix_LoadWAV error: {0}\n", Mix_GetError());
    }
    m_attackMissSound = Mix_LoadWAV(fmt::format("{0}/sounds/attack_miss.wav", m_resourcesPath).c_str());
    if (m_attackMissSound == nullptr) {
        std::cerr << fmt::format("Mix_LoadWAV error: {0}\n", Mix_GetError());
    }
    m_attackCriticalSound = Mix_LoadWAV(fmt::format("{0}/sounds/critical.wav", m_resourcesPath).c_str());
    if (m_attackCriticalSound == nullptr) {
        std::cerr << fmt::format("Mix_LoadWAV error: {0}\n", Mix_GetError());
    }
    m_monsterAttackSound = Mix_LoadWAV(fmt::format("{0}/sounds/monster_attack.wav", m_resourcesPath).c_str());
    if (m_monsterAttackSound == nullptr) {
        std::cerr << fmt::format("Mix_LoadWAV error: {0}\n", Mix_GetError());
    }
    m_victorySound = Mix_LoadWAV(fmt::format("{0}/sounds/victory.wav", m_resourcesPath).c_str());
    if (m_victorySound == nullptr) {
        std::cerr << fmt::format("Mix_LoadWAV error: {0}\n", Mix_GetError());
    }
    m_bossDefeatSound = Mix_LoadWAV(fmt::format("{0}/sounds/bossDefeat.mp3", m_resourcesPath).c_str());
    if (m_bossDefeatSound == nullptr) {
        std::cerr << fmt::format("Mix_LoadWAV error: {0}\n", Mix_GetError());
    }
    m_levelUpSound = Mix_LoadWAV(fmt::format("{0}/sounds/levelup.wav", m_resourcesPath).c_str());
    if (m_levelUpSound == nullptr) {
        std::cerr << fmt::format("Mix_LoadWAV error: {0}\n", Mix_GetError());
    }
}

GLBattleWindow::~GLBattleWindow() {
    Mix_FreeChunk(m_attackSound);
    Mix_FreeChunk(m_attackMissSound);
    Mix_FreeChunk(m_attackCriticalSound);
    Mix_FreeChunk(m_monsterAttackSound);
    Mix_FreeChunk(m_victorySound);
    Mix_FreeChunk(m_bossDefeatSound);
    Mix_FreeChunk(m_levelUpSound);
}

bool GLBattleWindow::initBattleShaders(const std::string &resourcesPath) {
    if (!m_bossDefeatOverlay.initialize(resourcesPath)) {
        m_lastError = m_bossDefeatOverlay.getLastError();
        return false;
    }
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
    m_experienceObtained = 0;
    m_didLevelUp = false;
    m_bossFlashOpacity = 0.0F;
    m_bossDefeatOpacity = 1.0F;
    m_bossDefeatShake = 0.0F;
    m_bossDefeatSoundPlayed = false;
    m_victorySoundPlayed = false;
    m_levelUpSoundPlayed = false;
}

void GLBattleWindow::update() {
    if (m_namedObjectsAnimations.contains(MonsterHPBarObj)) {
        m_namedObjectsAnimations[MonsterHPBarObj]->process();
    }
    if (m_namedObjectsAnimations.contains(MonsterShaking)) {
        m_namedObjectsAnimations[MonsterShaking]->process();
    }
    if (m_namedObjectsAnimations.contains(PlayerHPShaking)) {
        m_namedObjectsAnimations[PlayerHPShaking]->process();
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
                case BattleAction::BossDefeat:
                    bossDefeatWorkflow();
                    break;
                case BattleAction::PlayerWon:
                    playerWonWorkflow();
                    break;
                case BattleAction::PlayerGetReward:
                    playerObtainRewardWorkflow();
                    break;
                case BattleAction::PlayerLevelUp:
                    playerObtainNewLevelWorkflow();
                    break;
                case BattleAction::PlayerDied:
                    playerDiedWorkflow();
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
    GLPopupWindow::freeGLObjects(m_monsterHPBarWindow);
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
    // HP Stat Text
    addXCenteredTextObject({fmt::format("HP {0}/{1}",
                                m_glPlayer->getStats().health ,
                                m_glPlayer->getStats().maxHealth),
                            {1.0F, 140.0F},
                           0.5F},
                           30.0F,
                           190.0F);
    m_glPlayerHPStat = m_glTextObjects.back();
    m_glTextObjects.pop_back();
    addXCenteredTextObject({"MP 0/0", {1.0F, 180.0F}, 0.5F}, 30.0F, 190.0F);
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
    GLPopupWindow::freeGLObjects(m_namedObjects);
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
    drawPlayerHPStat(m_glPlayerHPStat);
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
    if (m_monster->getType() == MonsterType::Boss && m_monster->isDead()) {
        monsterTransparency = m_bossDefeatOpacity;
    }
    if (monsterTransparency > 0.0F || !m_monster->isDead()) {
        drawMonster(m_namedObjects.at(MonsterObj), m_namedObjects.at(MonsterObj).textureGLId, monsterTransparency);
    }
    if (m_monster->getType() == MonsterType::Boss && m_monster->isDead() && m_bossDefeatOpacity <= 0.0F) {
        return;
    }
    for (const auto &obj : m_monsterHPBarWindow) {
        m_glFormService->drawQuad(obj, m_windowGLTexture.glTextureId, monsterTransparency);
    }
    drawMonsterHPBar(m_namedObjects.at(MonsterHPBarObj), m_namedObjects.at(MonsterHPBarObj).textureGLId, monsterTransparency);
    m_bossDefeatOverlay.render(m_bossFlashOpacity, 1.0F);
}

void GLBattleWindow::gameWindowSizeChanged(const Size<> &size) {
    GLPopupWindow::gameWindowSizeChanged(size);
    if (m_monster) {
        generateGLElements();
    }
}

void GLBattleWindow::prepareWindow(const std::string &id) {
    reset();
    auto monster = m_monsterStore->findMonster(id);
    if (!monster) {
        throw std::runtime_error(fmt::format("Unable to found the monster {0}", id));
    }
    m_monster = std::make_unique<Monster>(*monster);
    // Randomize the monster health
    std::uniform_int_distribution<> distributionMonsterHealth(m_monster->getHealthRange().first,
                                                              m_monster->getHealthRange().second);
    int health = distributionMonsterHealth(RandomGenerator::instance());
    m_monster->setMaxHealth(health);
    m_monster->setHealth(health);
    setTitle(m_monster->getName());
    addBattleLog(fmt::format("You encountered a {}!", m_monster->getName()).c_str());
    generateGLElements();
    m_inputDevicesState->invalidateDirections();
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
        addBattleLog("<Not implemented yet>");
    } else if (m_menuActionsPosition == 2) {  // Item
        addBattleLog("<Not implemented yet>");
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
    // If player's attack is >= than monster defense 1 on 20 to miss else
    // PlayerAttack on MonsterDefense to have success (otherwise miss).
    bool playerMissed = false;
    if (m_glPlayer->getStats().attack >= m_monster->getDefense()) {
        std::uniform_int_distribution<> distributionMissChance(1, 20);
        if (distributionMissChance(RandomGenerator::instance()) == 1) {
            playerMissed = true;
        }
    } else {
        std::uniform_int_distribution<> distributionMissChance(1, static_cast<int>(ceil(m_monster->getDefense())));
        if (static_cast<float>(distributionMissChance(RandomGenerator::instance())) > m_glPlayer->getStats().attack) {
            playerMissed = true;
        }
    }
    // Compute the potential damage
    // 1 on 16 to land a critical
    bool critical = false;
    float criticalBonus = 1.0F;
    std::uniform_int_distribution<> distributionCriticalChance(1, 16);
    if (distributionCriticalChance(RandomGenerator::instance()) == 1) {
        // Critical bonus from 1.5 to 2.0
        critical = true;
        std::uniform_real_distribution<> distributionCriticalBonus(1.5, 2.0);
        criticalBonus = static_cast<float>(distributionCriticalBonus(RandomGenerator::instance()));
    }
    std::uniform_real_distribution<> distributionRandomRoll(0.75, 1.0);
    float randomRollValue = static_cast<float>(distributionRandomRoll(RandomGenerator::instance()));
    int damage = static_cast<int>(ceil(m_glPlayer->getStats().attack * criticalBonus * randomRollValue - m_monster->getDefense()));
    if (playerMissed || damage <= 0) {
        Mix_PlayChannel(-1, m_attackMissSound, 0);
        addBattleLog("You missed your attack!");
        startAction(BattleAction::MonsterTurn, 500);
    } else {
        m_monster->reduceHealth(damage);
        if (critical) {
            Mix_PlayChannel(-1, m_attackCriticalSound, 0);
            m_namedObjectsAnimations[MonsterShaking] = std::make_shared<ShakingAnimation>(60, 4);
            addBattleLog(fmt::format("You struck critically! -{0} HP!", damage).c_str());
        } else {
            Mix_PlayChannel(-1, m_attackSound, 0);
            m_namedObjectsAnimations[MonsterShaking] = std::make_shared<ShakingAnimation>(25, 3);
            addBattleLog(fmt::format("You hit and HPs have been reduced by {0}!", damage).c_str());
        }
        float oldMonsterHealthRatio = m_monster->getHealthRatio();
        m_namedObjectsAnimations[MonsterHPBarObj] = std::make_shared<ValueChangeAnimation>(oldMonsterHealthRatio,
                m_monster->getHealthRatio(),
                0.075F);
        if (m_monster->isDead()) {
            m_namedObjectsAnimations[MonsterObj] = std::make_shared<ValueChangeAnimation>(1.0F, 0.1F, 0.05F);
            addBattleLog(fmt::format("You have defeated the {0}", m_monster->getName()).c_str());
            std::uniform_int_distribution<> distributionGoldReward(m_monster->getGoldRewardRange().first,
                                                               m_monster->getGoldRewardRange().second);
            m_goldObtained = distributionGoldReward(RandomGenerator::instance());
            if (m_goldObtained > 0) {
                m_glPlayer->addGold(m_goldObtained);
                m_namedObjectsAnimations[MoreTextObj] = std::make_shared<FadeLoopAnimation>(0.1F, 1.0F, 0.01F);
            }
            std::uniform_int_distribution<> distributionExpReward(m_monster->getExperienceRewardRange().first,
                                                               m_monster->getExperienceRewardRange().second);
            m_experienceObtained = distributionExpReward(RandomGenerator::instance());
            unsigned int oldLevel = m_glPlayer->getLevel();
            m_glPlayer->addExperience(m_experienceObtained);
            m_didLevelUp = oldLevel != m_glPlayer->getLevel();
            if (m_monster->getType() == MonsterType::Boss) {
                startAction(BattleAction::BossDefeat, 1500);
            } else {
                startAction(BattleAction::PlayerWon, 1000);
            }
        } else {
            startAction(BattleAction::MonsterTurn, 500);
        }
    }
}

void GLBattleWindow::playerRunWorkflow() {
    if (m_currentBattleAction == BattleAction::PlayerTryToRun) {
        // If player's attack is >= than monster defense then 9 on 10 to run away
        // else one chance MonsterDefense-PlayerAttack to a maximum of 10.
        bool runAway = false;
        // You cannot run away against a boss
        if (m_monster->getType() != MonsterType::Boss) {
            float difference = m_monster->getDefense() - m_glPlayer->getStats().attack;
            if (difference > 10.0F) {
                difference = 10.0F;
            }
            if (difference <= 0.0F) {
                std::uniform_int_distribution<> distributionAttemptingToRun(1, 10);
                if (distributionAttemptingToRun(RandomGenerator::instance()) > 1) {
                    runAway = true;
                }
            } else {
                std::uniform_int_distribution<> distributionAttemptingToRun(1, static_cast<int>(ceil(difference)));
                if (distributionAttemptingToRun(RandomGenerator::instance()) == 1) {
                    runAway = true;
                }
            }
        }
        if (runAway) {
            startAction(BattleAction::PlayerRanAway, 750);
            addBattleLog("You ran away!");
        } else {
            m_currentBattleAction = BattleAction::PlayerTurn;
            m_menuActionsPosition = 0;
            addBattleLog("Couldn't escape! Battle continues...");
            startAction(BattleAction::MonsterTurn, 500);
        }
    } else if (m_currentBattleAction == BattleAction::PlayerRanAway) {
        m_battleCompleted();
    }
}

void GLBattleWindow::bossDefeatWorkflow() {
    if (!m_bossDefeatSoundPlayed) {
        m_namedObjectsAnimations.erase(MonsterShaking);
        startAction(BattleAction::BossDefeat, 0);
        if (m_bossDefeatSound != nullptr) {
            Mix_PlayChannel(-1, m_bossDefeatSound, 0);
        }
        m_bossDefeatSoundPlayed = true;
    }
    constexpr Uint64 FlashDuration = 1220;
    constexpr Uint64 FlashInterval = 1040;
    constexpr Uint64 FlashesDuration = FlashInterval * 2;
    constexpr Uint64 FadeDuration = 2100;
    const Uint64 elapsed = SDL_GetTicks64() - m_actionStepStartTicks;
    if (elapsed < FlashesDuration) {
        const Uint64 flashElapsed = elapsed % FlashInterval;
        m_bossFlashOpacity = flashElapsed < FlashDuration ?
            1.0F - static_cast<float>(flashElapsed) / static_cast<float>(FlashDuration) : 0.0F;
        return;
    }
    m_bossFlashOpacity = 0.0F;
    const Uint64 fadeElapsed = elapsed - FlashesDuration;
    if (fadeElapsed < FadeDuration) {
        m_bossDefeatOpacity = 1.0F - static_cast<float>(fadeElapsed) / static_cast<float>(FadeDuration);
        m_bossDefeatShake = (fadeElapsed / 30 % 2 == 0 ? 14.0F : -14.0F) * m_bossDefeatOpacity;
        return;
    }
    m_bossDefeatOpacity = 0.0F;
    m_bossDefeatShake = 0.0F;
    startAction(BattleAction::PlayerWon, 0);
}

void GLBattleWindow::playerWonWorkflow() {
    if (!m_victorySoundPlayed && m_monster->getType() != MonsterType::Boss) {
        Mix_PlayChannel(-1, m_victorySound, 0);
        m_victorySoundPlayed = true;
    }
    if (m_namedObjectsAnimations.contains(MoreTextObj)) {
        m_namedObjectsAnimations[MoreTextObj]->process();
    }
    m_namedObjectsAnimations[MonsterObj]->process();
    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        if (!m_didLevelUp) {
            m_namedObjectsAnimations.erase(MoreTextObj);
        }
        m_currentBattleAction = BattleAction::PlayerGetReward;
        addBattleLog(fmt::format("You obtain {0} gold and {1} experience!", m_goldObtained, m_experienceObtained).c_str());
    }
}

void GLBattleWindow::playerObtainRewardWorkflow() {
    if (!m_namedObjectsAnimations[MonsterObj]->isCompleted()) {
        m_namedObjectsAnimations[MonsterObj]->process();
    }
    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        if (m_didLevelUp) {
            m_namedObjectsAnimations.erase(MoreTextObj);
            m_currentBattleAction = BattleAction::PlayerLevelUp;
            addBattleLog(fmt::format("You have reach level {0}! Stronger than ever!", m_glPlayer->getLevel()).c_str());
        } else {
            m_battleCompleted();
        }
    }
}

void GLBattleWindow::playerObtainNewLevelWorkflow() {
    if (!m_levelUpSoundPlayed) {
        Mix_PlayChannel(-1, m_levelUpSound, 0);
        m_levelUpSoundPlayed = true;
    }
    if (!m_namedObjectsAnimations[MonsterObj]->isCompleted()) {
        m_namedObjectsAnimations[MonsterObj]->process();
    }
    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        m_battleCompleted();
    }
}

void GLBattleWindow::playerDiedWorkflow() {
    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        //TODO: Implement the process when you die in a battle
        throw std::runtime_error("You died");
    }
}

void GLBattleWindow::monsterTurnWorkflow() {
    // Decide if the monster attack, cast a spell or heal itself
    startAction(BattleAction::MonsterAttack, 500);
    addBattleLog(fmt::format("The {0} attack...", m_monster->getName()).c_str());
    generateGLElements();
}

void GLBattleWindow::monsterAttackWorkflow() {
    // If monster's attack is >= than player defense 1 on 20 to miss else
    // MonsterAttack on PlayerDefense to have success (otherwise miss).
    bool monsterMissed = false;
    if (m_monster->getAttack() >= m_glPlayer->getStats().defense) {
        std::uniform_int_distribution<> distributionMissChance(1, 20);
        if (distributionMissChance(RandomGenerator::instance()) == 1) {
            monsterMissed = true;
        }
    } else {
        std::uniform_int_distribution<> distributionMissChance(1, static_cast<int>(ceil(m_glPlayer->getStats().defense)));
        if (static_cast<float>(distributionMissChance(RandomGenerator::instance())) > m_monster->getAttack()) {
            monsterMissed = true;
        }
    }
    // Compute the potential damage
    // 1 on 16 to land a critical
    bool critical = false;
    float criticalBonus = 1.0F;
    std::uniform_int_distribution<> distributionCriticalChance(1, 32);
    if (distributionCriticalChance(RandomGenerator::instance()) == 1) {
        // Critical bonus from 1.5 to 2.0
        critical = true;
        std::uniform_real_distribution<> distributionCriticalBonus(1.3, 1.5);
        criticalBonus = static_cast<float>(distributionCriticalBonus(RandomGenerator::instance()));
    }
    std::uniform_real_distribution<> distributionRandomRoll(0.75, 1.0);
    float randomRollValue = static_cast<float>(distributionRandomRoll(RandomGenerator::instance()));
    int damage = static_cast<int>(ceil((m_monster->getAttack() * criticalBonus * randomRollValue) - (m_glPlayer->getStats().defense / 2)));
    if (monsterMissed || damage <= 0) {
        Mix_PlayChannel(-1, m_attackMissSound, 0);
        startAction(BattleAction::PlayerTurn, 500);
        addBattleLog(fmt::format("The {0} missed its attack!", m_monster->getName()).c_str());
    } else {
        Mix_PlayChannel(-1, m_monsterAttackSound, 0);
        m_namedObjectsAnimations[MonsterObj] = std::make_shared<ValueChangeAnimation>(1.0F, 0.1F, 0.05F);
        m_glPlayer->reduceHealth(damage);
        if (critical) {
            m_namedObjectsAnimations[PlayerHPShaking] = std::make_shared<ShakingAnimation>(60, 4);
            addBattleLog(fmt::format("{0} landed a crit -{1} HP lost!", m_monster->getName(), damage).c_str());
        } else {
            m_namedObjectsAnimations[PlayerHPShaking] = std::make_shared<ShakingAnimation>(25, 3);
            addBattleLog(fmt::format("{0} hit! You lost {1} HP!", m_monster->getName(), damage).c_str());
        }
        if (m_glPlayer->isDead()) {
            addBattleLog("You are dead...");
            startAction(BattleAction::PlayerDied, 500);
        } else {
            startAction(BattleAction::PlayerTurn, 500);
            generateGLElements();
        }
    }
}

void GLBattleWindow::drawPlayerHPStat(const GLTextObject &textObject) {
    GLTextObject textObjectToDraw = textObject;
    if (m_namedObjectsAnimations.contains(PlayerHPShaking) &&
        !m_namedObjectsAnimations.at(PlayerHPShaking)->isCompleted()) {
        textObjectToDraw.position.setX(
                textObjectToDraw.position.x() +
                m_namedObjectsAnimations.at(PlayerHPShaking)->getValue());
    }
    m_glFormService->drawText(textObjectToDraw);
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
    translationX += m_bossDefeatShake;
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
