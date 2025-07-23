#pragma once

#include <SDL2/SDL_events.h>
#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "gameMap.hpp"
#include "gameMapModeController.hpp"
#include "glCharacterWindow.hpp"
#include "glChoicePopup.hpp"
#include "glFormService.hpp"
#include "glInventory.hpp"
#include "glPlayer.hpp"
#include "glTextBox.hpp"
#include "glTextService.hpp"
#include "glTextureService.hpp"
#include "glTile.hpp"
#include "glTileService.hpp"
#include "inputDevicesState.hpp"
#include "mapTile.hpp"
#include "mapTileTrigger.hpp"
#include "point.hpp"
#include "size.hpp"
#include "tileSize.hpp"

namespace thewarrior::ui {

enum GameMapInputMode {
    Map,
    MainMenuPopup,
    CharacterWindow,
    InventoryWindow
};

class GameMapMode {
 public:
    GameMapMode();
    void initialize(const std::string &resourcesPath,
            std::shared_ptr<GLPlayer> glPlayer,
            std::shared_ptr<thewarrior::models::ItemStore> itemStore,
            std::shared_ptr<thewarrior::ui::models::MessagePipeline> messagePipeline,
            std::shared_ptr<GLTileService> tileService,
            std::shared_ptr<GLTextBox> textBox,
            std::shared_ptr<GLTextService> textService,
            const std::map<std::string, unsigned int> *texturesGLItemStore,
            std::shared_ptr<InputDevicesState> inputDevicesState);
    bool initShaders(const std::string &resourcesPath);
    const std::string &getLastError() const;
    void processEvents(SDL_Event &e);
    void update();
    void render();
    void unloadGLMapObjects();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
    void gameWindowTileSizeChanged(const TileSize &tileSize);

 private:
    thewarrior::ui::controllers::GameMapModeController m_controller;
    std::string m_lastError = "";
    std::string m_resourcesPath = "";
    std::string m_currentMapName = "";
    GameMapInputMode m_inputMode = GameMapInputMode::Map;
    std::shared_ptr<thewarrior::models::GameMap> m_map;
    std::shared_ptr<GLPlayer> m_glPlayer;
    std::shared_ptr<GLTileService> m_tileService;
    GLTextureService m_textureService;
    std::shared_ptr<GLTextBox> m_textBox;
    std::shared_ptr<GLShaderProgram> m_shaderProgram = nullptr;
    std::shared_ptr<GLFormService> m_glFormService = std::make_shared<GLFormService>();
    GLCharacterWindow m_glCharacterWindow;
    GLInventory m_glInventory;
    GLChoicePopup m_choicePopup;
    thewarrior::models::Size<> m_screenSize = {1, 1};
    std::vector<GLTile> m_glTiles;
    std::map<std::string, unsigned int> m_texturesGLMap;
    TileSize m_tileSize = { 1.0F, 1.0F, 1.0F };
    std::array<int, 4> m_tileCoordToDisplay = { 0, 0, 0, 0 };  // Left, Right, Up and Bottom
    std::shared_ptr<InputDevicesState> m_inputDevicesState = nullptr;
    GLfloat m_texColorBuf[4][3] = { { 1.0F, 1.0F, 1.0F },   /* Red */
        { 1.0F, 1.0F, 1.0F },   /* Green */
        { 1.0F, 1.0F, 1.0F },   /* Blue */
        { 1.0F, 1.0F, 1.0F } };
    bool m_blockKeyDown = false;
    bool m_isCharacterWindowDisplayed = false;
    bool m_isInventoryDisplayed = false;
    void showMainMenu();
    void toggleInventoryWindow();
    void toggleCharacterWindow();
    void generateGLMapObjects();
    void drawObjectTile(GLTile &tile);
    void actionButtonPressed();
    void moveUpPressed();
    void moveDownPressed();
    void moveLeftPressed();
    void moveRightPressed();
    void processAction(thewarrior::models::MapTileTriggerAction action,
                       const std::map<std::string, std::string> &properties,
                       thewarrior::models::MapTile *tile = nullptr,
                       thewarrior::models::Point<int> tilePosition = thewarrior::models::Point<int>(0, 0));
    void loadMap(const std::string &filePath, const std::string &mapName);
    void changeMap(const std::string &filePath, const std::string &mapName);
    void calculateGLTileCoord(const thewarrior::models::Point<> &tilePosition, GLfloat tileCoord[4][2]);
    void calculateTilesToDisplay();
    void loadMapTextures();
    void onCharacterWindowClose();
    void onInventoryWindowClose();
    void mainMenuPopupClicked(size_t choice);
    void mainMenuPopupCanceled();
};

}  // namespace thewarrior::ui
