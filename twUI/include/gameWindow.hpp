#pragma once

#include "fpsCalculator.hpp"
#include "glPlayer.hpp"
#include "glTile.hpp"
#include "glTileService.hpp"
#include "glTextService.hpp"
#include "gameMap.hpp"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct GenerateGLObjectInfo {
    const Texture *lastUsedTexture;
    GLuint *vao;
    const std::string &textureName;
    const int textureIndex;
    GLuint *vboPosition;
    GLuint *vboColor;
    GLuint *vboTexture;
};

class GameWindow
{
public:
    GameWindow(const std::string &title,
               int x, int y,
               int width, int height);
    ~GameWindow();
    void update(double delta_time);
    void render();
    void show();
    void hide();
    bool isAlive() const;
    void processEvents();
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_GLContext gContext;
    int width;
    int height;
    bool mustExit;
    std::string executablePath;
    std::string resourcesPath;
    GLTileService tileService;
    GLTextService textService;
    std::vector<GLTile> glTiles;
    GLPlayer glPlayer;
    std::shared_ptr<GameMap> map; 
    std::map<std::string, unsigned int> texturesGLMap;
    GLfloat tileCoordBuf[4][2];
    GLfloat texCoordBuf[4][2];
    GLfloat texColorBuf[4][3];
    float TILEWIDTH { 0.1f };
    float TILEHALFWIDTH { TILEWIDTH / 2.0f };
    float TILEHALFHEIGHT;
    //FPS variables
    FPSCalculator fpsCalculator;
    bool toggleFPS;
    bool blockToggleFPS;
    SDL_Joystick *joystick;
    const std::string &getExecutablePath();
    const std::string &getResourcesPath();
    void moveUpPressed();
    void moveDownPressed();
    void moveLeftPressed();
    void moveRightPressed();
    void calculateTileSize();
    void generateGLMapObjects();
    void unloadGLMapObjects();
    void generateGLObject(GenerateGLObjectInfo &info, const GLfloat tileCoord[4][2], const GLfloat colors[4][3]);
    void generateGLPlayerObject();
    void unloadGLPlayerObject();
    std::string loadShaderFile(const std::string &file);
    void loadMap(const std::string &filePath);
    void changeMap(const std::string &filePath);
    void processAction(TileAction action, const std::map<std::string, std::string> &properties);
    void loadTextures();
    void setTextureUVFromIndex(const Texture *texture, GLfloat uvMap[4][2], int index);
    void setTileCoordToOrigin();
    void setPlayerPosition();
    void setPlayerTexture();
    void drawPlayer();
    void drawObjectTile(GLTile &tile);
};