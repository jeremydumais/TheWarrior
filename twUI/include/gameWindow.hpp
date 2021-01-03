#pragma once

#include "glPlayer.hpp"
#include "glTile.hpp"
#include "gameMap.hpp"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

//enum class TileMode { Texture, Object };
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
    void render();
    void show();
    void hide();
    bool isAlive() const;
    void processEvents();
private:
    SDL_Window *window;
    SDL_GLContext gContext;
    int width;
    int height;
    bool mustExit;
    std::string executablePath;
    std::string resourcesPath;
    GLuint vertexshader;
    GLuint fragmentshader;
    GLuint shaderprogram;
    std::string vertexShaderContent;
    std::string fragmentShaderContent;
    std::vector<GLTile> glTiles;
    GLPlayer glPlayer;
    std::shared_ptr<GameMap> map; 
    std::map<std::string, unsigned int> texturesGLMap;
    GLfloat uv[4][2];
    float TILEWIDTH { 0.1f };
    float TILEHALFWIDTH { TILEWIDTH / 2.0f };
    float TILEHALFHEIGHT;
    void generateGLMapObjects();
    void generateGLObject(GenerateGLObjectInfo &info, const GLfloat tileCoord[4][2], const GLfloat colors[4][3]);
    void generateGLPlayerObject();
    std::string loadShaderFile(const std::string &file);
    bool compileShaders();
    void linkShaders();
    void loadMap(const std::string &filePath);
    void loadTextures();
    void setTextureUVFromIndex(const Texture *texture, GLfloat uvMap[4][2], int index);
    void setPlayerPosition(int x, int y);
    const std::string &getExecutablePath();
    const std::string &getResourcesPath();
};