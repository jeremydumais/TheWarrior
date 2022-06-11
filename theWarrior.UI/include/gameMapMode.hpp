#pragma once

#include "gameMap.hpp"
#include "gameMapModeController.hpp"
#include "glPlayer.hpp"
#include "glTextBox.hpp"
#include "glTextureService.hpp"
#include "glTile.hpp"
#include "glTileService.hpp"
#include "mapTile.hpp"
#include "mapTileTrigger.hpp"
#include "point.hpp"
#include "size.hpp"
#include "tileSize.hpp"
#include <SDL2/SDL_events.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

class GameMapMode
{
public:
    GameMapMode();
    void initialize(const std::string &resourcesPath,
                    std::shared_ptr<GLPlayer> glPlayer,
                    std::shared_ptr<ItemStore> itemStore, 
                    std::shared_ptr<MessagePipeline> messagePipeline,
                    std::shared_ptr<GLTileService> tileService,
                    std::shared_ptr<GLTextBox> textBox,
                    SDL_Joystick *joystick);
    void render();
    void processEvents(SDL_Event &e);
    void generateGLMapObjects();
    void unloadGLMapObjects();
    void gameWindowSizeChanged(const Size<> &size);
    void gameWindowTileSizeChanged(const TileSize &tileSize);
private:
    GameMapModeController m_controller;
    std::string m_resourcesPath;
    std::shared_ptr<GameMap> m_map;
    std::shared_ptr<GLPlayer> m_glPlayer;
    std::shared_ptr<GLTileService> m_tileService;
    GLTextureService m_textureService;
    std::shared_ptr<GLTextBox> m_textBox;
    Size<> m_screenSize;
    std::vector<GLTile> m_glTiles;
    std::map<std::string, unsigned int> m_texturesGLMap;
    TileSize m_tileSize;
    SDL_Joystick *m_joystick;
    GLfloat m_texColorBuf[4][3];
    bool m_blockKeyDown;
    void drawObjectTile(GLTile &tile);
    void actionButtonPressed();
    void moveUpPressed();
    void moveDownPressed();
    void moveLeftPressed();
    void moveRightPressed();
    void processAction(MapTileTriggerAction action, const std::map<std::string, std::string> &properties, MapTile *tile = nullptr, Point<> tilePosition = Point(0, 0));
    void loadMap(const std::string &filePath);
    void changeMap(const std::string &filePath);
    void calculateGLTileCoord(const Point<> &tilePosition, GLfloat tileCoord[4][2]);
    void loadMapTextures();

};