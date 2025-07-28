#pragma once

#include <SDL2/SDL_events.h>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <boost/signals2.hpp>
#include "IAnimation.hpp"
#include "glObjectService.hpp"
#include "glPlayer.hpp"
#include "glPopupWindow.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "inputDevicesState.hpp"
#include "monster.hpp"
#include "monsterStore.hpp"

namespace thewarrior::ui {

enum class BattleAction {
    PlayerTurn,
    PlayerAttack,
    PlayerItem,
    PlayerSpell,
    PlayerTryToRun,
    PlayerRanAway,
    PlayerWon,
    PlayerGetReward,
    MonsterTurn,
    MonsterAttack,
    MonsterSpell,
    MonsterItem,
    MonsterRun
};

constexpr const char* MoreTextObj = "moreTextObj";
constexpr const char* MonsterObj = "monsterObj";
constexpr const char* MonsterHPWindowObj = "monsterHPWindowObj";
constexpr const char* MonsterHPBarObj = "monsterHPBarObj";

class GLBattleWindow : public GLPopupWindow {
 public:
    GLBattleWindow();
    ~GLBattleWindow() override = default;
    void
        initialize(const std::string &resourcePath,
                std::shared_ptr<GLPlayer> glPlayer,
                std::shared_ptr<GLTextService> textService,
                std::shared_ptr<thewarrior::models::MonsterStore> monsterStore,
                const std::map<std::string, unsigned int> *texturesGLMonsterStore,
                std::shared_ptr<InputDevicesState> inputDevicesState);
    bool initBattleShaders(const std::string &resourcesPath);
    void reset();
    void update();
    void generateGLElements();
    void render();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
    void prepareWindow(const std::string &id);
    boost::signals2::signal<void()> m_battleCompleted;

 protected:
    std::shared_ptr<GLShaderProgram> m_monsterHealthShaderProgram;
    std::shared_ptr<GLPlayer> m_glPlayer;
    std::shared_ptr<thewarrior::models::MonsterStore> m_monsterStore;
    const std::map<std::string, unsigned int> *m_texturesGLMonsterStore;
    std::shared_ptr<InputDevicesState> m_inputDevicesState = nullptr;
    GLTexture m_slotsGLTexture;
    std::map<std::string, GLObject> m_namedObjects = {};
    std::map<std::string, std::shared_ptr<IAnimation>> m_namedObjectsAnimations = {};
    std::vector<GLObject> m_monsterHPBarWindow = {};
    std::unique_ptr<thewarrior::models::Monster> m_monster;
    std::unique_ptr<thewarrior::models::Monster> m_monsterInitial;
    std::vector<GLTextObject> m_glTextActions;
    size_t m_menuActionsPosition;
    std::queue<std::string> m_battleLog;
    Uint64 m_lastMoveUpTicks = 0;
    Uint64 m_lastMoveDownTicks = 0;
    Uint64 m_actionStepStartTicks = 0;
    Uint64 m_actionStepNextTicks = 0;
    int m_goldObtained = 0;
    BattleAction m_currentBattleAction = BattleAction::PlayerTurn;
    void moveUpPressed();
    void moveDownPressed();
    void actionButtonPressed();
    void addBattleLog(const std::string &log);
    void startAction(BattleAction action, Uint64 timeLength);
    void playerAttackWorkflow();
    void playerRunWorkflow();
    void playerWonWorkflow();
    void playerObtainRewardWorkflow();
    void monsterTurnWorkflow();
    void monsterAttackWorkflow();
    void drawMonsterHPBar(const GLObject &glObject, GLuint textureGLIndex, float transparency);
};

}  // namespace thewarrior::ui
