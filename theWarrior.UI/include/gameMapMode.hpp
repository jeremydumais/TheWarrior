#pragma once

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mixer.h>
#include <array>
#include <map>
#include <memory>
#include <ranges>
#include <string>
#include <vector>
#include <boost/signals2.hpp>
#include "conversationController.hpp"
#include "conversationScenario.hpp"
#include "gameMap.hpp"
#include "gameMapModeController.hpp"
#include "gameState.hpp"
#include "glBattleWindow.hpp"
#include "glCharacterWindow.hpp"
#include "glChoicePopup.hpp"
#include "glFormService.hpp"
#include "glInventory.hpp"
#include "glMerchantShop.hpp"
#include "glSellItems.hpp"
#include "glNPC.hpp"
#include "glPlayer.hpp"
#include "glScreenOverlay.hpp"
#include "glTextBox.hpp"
#include "glTextService.hpp"
#include "glTextureService.hpp"
#include "glTile.hpp"
#include "glTileService.hpp"
#include "inputDevicesState.hpp"
#include "mapTile.hpp"
#include "mapTileTrigger.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include "point.hpp"
#include "size.hpp"
#include "tileSize.hpp"

namespace thewarrior::ui {

enum class GameMapInputMode {
    Map,
    MainMenuPopup,
    ConversationChoice,
    CharacterWindow,
    InventoryWindow,
    MerchantShop,
    SellItems,
    Battle
};

enum class SleepSequenceState {
    Inactive,
    FadingOut,
    PlayingJingle,
    FadingIn
};

enum class ChangeMapSequenceState {
    Inactive,
    FadingOut,
    ChangeMap,
    FadingIn
};

class GameMapMode {
 public:
    GameMapMode();
    ~GameMapMode();
    bool initialize(const std::string &resourcesPath,
            const thewarrior::models::GameState &gameState,
            std::shared_ptr<GLTextService> textService,
            std::shared_ptr<InputDevicesState> inputDevicesState);
    bool initShaders(const std::string &resourcesPath);
    const std::string &getLastError() const;
    void processEvents(SDL_Event &e);
    void update();
    void render();
    void unloadGLMapObjects();
    void unloadGLNPCObjects();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
    void onGameWindowUpdate(float delta_time);
    bool areMonsterEncountersEnabled() const;
    boost::signals2::signal<void()> quitRequested;

 private:
    thewarrior::ui::controllers::GameMapModeController m_controller;
    thewarrior::ui::controllers::ConversationController m_conversationController;
    std::string m_lastError;
    GameMapInputMode m_inputMode = GameMapInputMode::Map;
    std::shared_ptr<thewarrior::models::GameMap> m_map;
    std::shared_ptr<GLPlayer> m_glPlayer = nullptr;
    std::vector<GLNPC> m_glNPCs;
    std::shared_ptr<GLTileService> m_tileService = std::make_shared<GLTileService>();
    GLTextureService m_textureService;
    std::shared_ptr<GLTextBox> m_textBox = std::make_shared<GLTextBox>();
    std::shared_ptr<GLShaderProgram> m_shaderProgram = nullptr;
    std::shared_ptr<GLFormService> m_glFormService = std::make_shared<GLFormService>();
    std::map<std::string, unsigned int> m_texturesGLItemStore;
    std::map<std::string, unsigned int> m_texturesGLMonsterStore;
    GLBattleWindow m_glBattleWindow;
    GLCharacterWindow m_glCharacterWindow;
    GLInventory m_glInventory;
    GLMerchantShop m_glMerchantShop;
    GLSellItems m_glSellItems;
    GLChoicePopup m_choicePopup;
    GLScreenOverlay m_screenOverlay;
    thewarrior::models::Size<> m_screenSize = {1, 1};
    std::vector<GLTile> m_glTiles;
    std::map<std::string, unsigned int> m_texturesGLMap;
    TileSize m_tileSize = { .tileWidth = 1.0F, .tileHalfWidth = 1.0F, .tileHalfHeight = 1.0F };
    std::array<int, 4> m_tileCoordToDisplay = { 0, 0, 0, 0 };  // Left, Right, Up and Bottom
    std::shared_ptr<InputDevicesState> m_inputDevicesState = nullptr;
    GLfloat m_texColorBuf[4][3] = { { 1.0F, 1.0F, 1.0F },   /* Red */
        { 1.0F, 1.0F, 1.0F },   /* Green */
        { 1.0F, 1.0F, 1.0F },   /* Blue */
        { 1.0F, 1.0F, 1.0F } };
    bool m_blockKeyDown = false;
    bool m_isCharacterWindowDisplayed = false;
    bool m_isInventoryDisplayed = false;
    bool m_monsterEncountersEnabled = true;
    // Sleep objects
    SleepSequenceState m_sleepSequenceState = SleepSequenceState::Inactive;
    float m_sleepOverlayOpacity = 0.0F;
    Mix_Music *m_sleepMusic = nullptr;
    bool m_restoreHealthAfterSleep = false;
    
    // Map change sequence
    ChangeMapSequenceState m_changeMapSequenceState = ChangeMapSequenceState::Inactive;
    float m_changeMapOverlayOpacity = 0.0F;
    std::map<std::string, std::string> m_changeMapProperties;

    Mix_Music* m_mapMusic = nullptr;
    Mix_Music* m_battleMusic = nullptr;
    thewarrior::models::Point<> getPlayerFacingTilePosition() const;
    void calculateTileSize();
    void showMainMenu();
    void toggleInventoryWindow();
    void toggleCharacterWindow();
    void generateGLMapObjects();
    void drawObjectTile(GLTile &tile);
    void actionButtonPressed();
    bool stopFacingNPCWandering();
    bool isTileOccupiedByVisibleNPC(const thewarrior::models::Point<> &position);
    void moveUpPressed();
    void moveDownPressed();
    void moveLeftPressed();
    void moveRightPressed();
    void processAction(thewarrior::models::MapTileTriggerAction action,
                       std::map<std::string, std::string> properties,
                       thewarrior::models::MapTile *tile = nullptr,
                       thewarrior::models::Point<int> tilePosition = thewarrior::models::Point<int>(0, 0));
    void processMapTileTrigger(
        const thewarrior::models::MapTileTrigger &trigger,
        thewarrior::models::MapTile *tile = nullptr,
        thewarrior::models::Point<int> tilePosition =
            thewarrior::models::Point<int>(0, 0));
    void checkForMonsterEncounter(const thewarrior::models::MapTile &tile);
    std::string selectMonsterEncounter(const std::vector<thewarrior::models::MonsterZoneMonsterEncounter> &encounters,
                                       thewarrior::models::MonsterEncounterRatio ratio);
    void loadMap(const std::string &filePath, const std::string &mapName);
    void changeMap(const std::string &filePath, const std::string &mapName);
    void updateMapChangeSequence(float deltaTime);
    void calculateGLTileCoord(const thewarrior::models::Point<> &tilePosition, GLfloat tileCoord[4][2]);
    void calculateTilesToDisplay();
    void loadMapTextures();
    bool loadStores();
    bool loadShaders();
    void loadItemStoreTextures();
    void loadMonsterStoreTextures();
    void onCharacterWindowClose();
    void onInventoryWindowClose();
    void mainMenuPopupClicked(size_t choice);
    void mainMenuPopupCanceled();
    void exitGameAndReturnToMainMenu();
    void onPlayerMoveCompleted();
    void onBattleCompleted();
    void completeCurrentMessage(bool allowTextReveal);
    void choicePopupClicked(std::size_t choiceIndex);
    void choicePopupCanceled();
    void restoreNPCDefaultBehavior(const std::string &npcId);
    void processCurrentConversationNode();
    void finishConversation();
    void failConversation(const std::string &error);
    void displayConversationDialogue(const thewarrior::models::ConversationDialogue &dialogue);
    void displayConversationChoice(const thewarrior::models::ConversationChoice &choice);
    void conversationChoiceSelected(std::size_t choiceIndex);
    bool executeConversationAction(const thewarrior::models::ConversationAction &action);
    bool completeConversationAction();
    bool followConversationActionFailure(const thewarrior::models::ConversationNodeTransition &transition);
    bool executeRestRequestedAction(const thewarrior::models::RestRequestedAction &action);
    bool executeMerchantShopAction(const thewarrior::models::MerchantShopAction &action);
    void onMerchantShopClose();
    bool executeSellItemsAction(const thewarrior::models::SellItemsAction &action);
    void onSellItemsClose();
    void keepConversationNPCStationary();
    void updateSleepSequence(float deltaTime);
    void restorePlayerAfterSleep();
    void completeSleepAction();




};

}  // namespace thewarrior::ui
