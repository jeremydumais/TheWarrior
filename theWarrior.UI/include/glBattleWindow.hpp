#pragma once

#include "glObjectService.hpp"
#include "glPlayer.hpp"
#include "glPopupWindow.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "inputDevicesState.hpp"
#include "monster.hpp"
#include "monsterStore.hpp"
#include <SDL2/SDL_events.h>
#include <boost/signals2.hpp>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

namespace thewarrior::ui {

enum class BattleAction {
  PlayerTurn,
  PlayerAttack,
  PlayerItem,
  PlayerSpell,
  PlayerTryToRun,
  PlayerRanAway,
  PlayerWon,
  MonsterTurn,
  MonsterAttack,
  MonsterSpell,
  MonsterItem,
  MonsterRun
};

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
  void reset();
  void update();
  void generateGLElements();
  void render();
  void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
  void prepareWindow(const std::string &id);
  boost::signals2::signal<void()> m_battleCompleted;

protected:
  std::shared_ptr<GLPlayer> m_glPlayer;
  std::shared_ptr<thewarrior::models::MonsterStore> m_monsterStore;
  const std::map<std::string, unsigned int> *m_texturesGLMonsterStore;
  std::shared_ptr<InputDevicesState> m_inputDevicesState = nullptr;
  GLTexture m_slotsGLTexture;
  std::map<std::string, GLObject> m_battleObjects = {};
  std::unique_ptr<thewarrior::models::Monster> m_monster;
  std::vector<GLTextObject> m_glTextActions;
  size_t m_menuActionsPosition;
  std::queue<std::string> m_battleLog;
  Uint64 m_lastMoveUpTicks = 0;
  Uint64 m_lastMoveDownTicks = 0;
  Uint64 m_actionStepStartTicks = 0;
  Uint64 m_actionStepNextTicks = 0;
  int m_battleMoreTextIconOpacity = 255;
  float moreTextIconAlphaValue = 0.0F;
  bool moreTextIconAlphaIncrease = true;
  float monsterAlphaValue = 1.0F;
  BattleAction m_battleAction = BattleAction::PlayerTurn;
  void moveUpPressed();
  void moveDownPressed();
  void actionButtonPressed();
  void addBattleLog(const std::string &log);
  void startAction(BattleAction action, Uint64 timeLength);
  void playerAttackWorkflow();
  void playerRunWorkflow();
  void playerWonWorkflow();
  void monsterTurnWorkflow();
  void monsterAttackWorkflow();
};

} // namespace thewarrior::ui
