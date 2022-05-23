#pragma once

#include "fpsCalculator.hpp"
#include "gameMap.hpp"
#include "gameWindowController.hpp"
#include "glObjectService.hpp"
#include "glPlayer.hpp"
#include "glTextBox.hpp"
#include "glTextService.hpp"
#include "glTextureService.hpp"
#include "glTile.hpp"
#include "glTileService.hpp"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

class GameWindow
{
public:
    GameWindow(const std::string &title,
               int x, int y,
               int width, int height);
    ~GameWindow();
    void update(float delta_time);
    void render();
    void show();
    void hide();
    bool isAlive() const;
    void processEvents();
private:
    GameWindowController m_controller;
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    SDL_GLContext m_gContext;
    int m_width;
    int m_height;
    bool m_mustExit;
    std::string m_executablePath;
    std::string m_resourcesPath;
    GLTileService m_tileService;
    GLTextBox m_textBox;
    GLTextService m_textService;
    GLTextureService m_textureService;
    std::vector<GLTile> m_glTiles;
    GLPlayer m_glPlayer;
    std::shared_ptr<GameMap> m_map; 
    std::map<std::string, unsigned int> m_texturesGLMap;
    std::map<std::string, unsigned int> m_texturesGLItemStore;
    GLfloat m_tileCoordBuf[4][2];
    GLfloat m_texCoordBuf[4][2];
    GLfloat m_texColorBuf[4][3];
    float TILEWIDTH { 0.1F };
    float TILEHALFWIDTH { TILEWIDTH / 2.0F };
    float TILEHALFHEIGHT;
    //FPS variables
    FPSCalculator m_fpsCalculator;
    bool m_toggleFPS;
    bool m_blockToggleFPS;
    SDL_Joystick *m_joystick;
    void moveUpPressed();
    void moveDownPressed();
    void moveLeftPressed();
    void moveRightPressed();
    void calculateTileSize();
    void generateGLMapObjects();
    void unloadGLMapObjects();
    void calculateGLTileCoord(const Point<> &tilePosition, GLfloat tileCoord[4][2]);
    void loadMap(const std::string &filePath);
    void changeMap(const std::string &filePath);
    void processAction(MapTileTriggerAction action, const std::map<std::string, std::string> &properties, MapTile *tile = nullptr, Point<> tilePosition = Point(0, 0));
    void loadMapTextures();
    void loadItemStoreTextures();
    void drawObjectTile(GLTile &tile);
};

