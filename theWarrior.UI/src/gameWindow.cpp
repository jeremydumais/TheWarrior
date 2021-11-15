#include "gameWindow.hpp"
#include "gameMapStorage.hpp"
#include <GL/glu.h>
#include <GL/glut.h>
#include <algorithm>
#include <boost/archive/binary_iarchive.hpp>
#include <cmath>
#include <fmt/format.h>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <libgen.h>         // dirname
#include <linux/limits.h>   // PATH_MAX
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <unistd.h>         // readlink

using namespace std;

GameWindow::GameWindow(const string &title,
                       int x, int y,
                       int width, int height) 
    : m_width(width),
      m_height(height),
      m_mustExit(false),
      m_texCoordBuf { { 0.0F, 0.0F },
                    { 0.0F, 0.0F }, 
                    { 0.0F, 0.0F }, 
                    { 0.0F, 0.0F } },
      m_texColorBuf { { 1.0F, 1.0F, 1.0F },   /* Red */
                    { 1.0F, 1.0F, 1.0F },   /* Green */
                    { 1.0F, 1.0F, 1.0F },   /* Blue */
                    { 1.0F, 1.0F, 1.0F } },
      m_toggleFPS(false),
      m_blockToggleFPS(false)
{
    //Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		cerr << fmt::format("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
	}

    //Use OpenGL 3.1 core
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    //Create window
    m_window = SDL_CreateWindow(title.c_str(), 
        x, 
        y, 
        width, 
        height, 
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED);
    if(m_window == NULL)
    {
        cerr << fmt::format("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    //Create context
    m_gContext = SDL_GL_CreateContext(m_window);
    if(m_gContext == nullptr)
    {
        cerr << fmt::format("OpenGL context could not be created! SDL Error: {0}\n", SDL_GetError());
        return;
    }

    //Initialize GLEW
    glewExperimental = GL_TRUE; 
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK )
    {
        cerr << fmt::format("Error initializing GLEW! {0}\n", glewGetErrorString(glewError));
        return;
    }

    //Use Vsync
    if( SDL_GL_SetSwapInterval(1) < 0 )
    {
        cerr << fmt::format("Warning: Unable to set VSync! SDL Error: {0}\n", SDL_GetError());
        return;
    }

    if (!m_tileService.initShader(fmt::format("{0}/shaders/tile_330_vs.glsl", getResourcesPath()),
                          fmt::format("{0}/shaders/tile_330_fs.glsl", getResourcesPath()))) {
        cerr << m_tileService.getLastError() << "\n";
        return;
    }
    if (!m_textService.initShader(fmt::format("{0}/shaders/text_330_vs.glsl", getResourcesPath()),
                          fmt::format("{0}/shaders/text_330_fs.glsl", getResourcesPath()))) {
        cerr << m_textService.getLastError() << "\n";
        return;
    }
    if (!m_textService.initFont(fmt::format("{0}/verdana.ttf", getResourcesPath()))) {
        cerr << m_textService.getLastError() << "\n";
        return;
    }
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    m_textService.setProjectionMatrix(projection);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    calculateTileSize();
    m_map = make_shared<GameMap>(1, 1);
    loadMap(fmt::format("{0}/maps/homeHouseV1.map", getResourcesPath()));
    loadTextures();
    generateGLMapObjects();
    m_glPlayer.initialize();
    generateGLPlayerObject();
    setPlayerPosition();

    m_fpsCalculator.initialize();

    SDL_JoystickEventState(SDL_ENABLE);
    m_joystick = SDL_JoystickOpen(0);
}

GameWindow::~GameWindow() 
{
    unloadGLMapObjects();
    unloadGLPlayerObject();
    SDL_JoystickClose(m_joystick);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void GameWindow::show() 
{
    SDL_ShowWindow(m_window);
}

void GameWindow::hide() 
{
    SDL_HideWindow(m_window);
}

bool GameWindow::isAlive() const
{
    return !m_mustExit;
}

void GameWindow::processEvents() 
{
    SDL_Event e;
    SDL_JoystickUpdate();
    
    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT){
            m_mustExit = true;
        }
        else if(e.type == SDL_KEYDOWN && !m_glPlayer.isInMovement()) {
            switch( e.key.keysym.sym )
            {
                case SDLK_UP:
                    moveUpPressed();
                    break;
                case SDLK_DOWN:
                    moveDownPressed();
                    break;
                case SDLK_LEFT:
                    moveLeftPressed();
                    break;
                case SDLK_RIGHT:
                    moveRightPressed();
                    break;
            };
        }
        else if(e.type == SDL_KEYUP) {
            if (m_blockToggleFPS) {
                m_blockToggleFPS = false;
            }
        }
        else if (e.type == SDL_JOYBUTTONDOWN) {
            if (e.jbutton.button == 0) {
                m_glPlayer.enableRunMode();
            }
        }
        else if (e.type == SDL_JOYBUTTONUP) {
            if (e.jbutton.button == 0) {
                m_glPlayer.disableRunMode();
            }
            if (e.jbutton.button == 1) {
                
                //Check if you are facing a tile with a ActionButton trigger configured.
                if (m_glPlayer.isFacing(PlayerFacing::Up)) {
                    Point tilePositionToProcess { m_glPlayer.coord.x(), m_glPlayer.coord.y() - 1 };
                    auto &tile = m_map->getTileForEditing(tilePositionToProcess);
                    auto actionButtonTrigger = tile.findConstTrigger(MapTileTriggerEvent::ActionButtonPressed);
                    if (actionButtonTrigger.has_value()) {
                        processAction(actionButtonTrigger->getAction(), actionButtonTrigger->getActionProperties(), &tile, tilePositionToProcess);
                    }
                }
            }
        }
        else if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_GetWindowSize(m_window, &m_width, &m_height);
                glViewport(0,0,m_width,m_height);
                calculateTileSize();
                unloadGLMapObjects();
                unloadGLPlayerObject();
                generateGLMapObjects();
                generateGLPlayerObject();
                setPlayerPosition();

                glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_width), 0.0f, static_cast<float>(m_height));
                m_textService.setProjectionMatrix(projection);
            }
        } 
    }
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    if ((keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL]) && keystate[SDL_SCANCODE_F]) {
        if (!m_blockToggleFPS) {
            m_toggleFPS = !m_toggleFPS;
            m_blockToggleFPS = true;
        }
    }

    for (int i = 0 ; i < SDL_JoystickNumHats(m_joystick) ; i++ ) {
        if (SDL_JoystickGetHat(m_joystick, i) == SDL_HAT_UP && !m_glPlayer.isInMovement()) {
            moveUpPressed();
            break;
        }
        else if (SDL_JoystickGetHat(m_joystick, i) == SDL_HAT_DOWN && !m_glPlayer.isInMovement()) {
            moveDownPressed();
            break;
        }
        else if (SDL_JoystickGetHat(m_joystick, i) == SDL_HAT_LEFT && !m_glPlayer.isInMovement()) {
            moveLeftPressed();
            break;
        }
        else if (SDL_JoystickGetHat(m_joystick, i) == SDL_HAT_RIGHT && !m_glPlayer.isInMovement()) {
            moveRightPressed();
            break;
        }
    }
    update(1.0f / 90.0f);
	render();
    if (m_toggleFPS) {
        m_fpsCalculator.calculate();
    }
	//SDL_GL_SwapWindow(window);
}

void GameWindow::moveUpPressed() 
{
    //Check if there is an action
    const auto tile = m_map->getTileFromCoord(m_glPlayer.coord);
    auto moveUpTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveUpPressed);
    if (moveUpTrigger.has_value()) {
        processAction(moveUpTrigger->getAction(), moveUpTrigger->getActionProperties());
    }
    else if (m_map->canSteppedOnTile(Point(m_glPlayer.coord.x(), m_glPlayer.coord.y() - 1))) {
        m_glPlayer.moveUp();
    } 
    m_glPlayer.faceUp();
    setPlayerTexture();
}

void GameWindow::moveDownPressed() 
{
    //Check if there is an action
    const auto tile = m_map->getTileFromCoord(m_glPlayer.coord);
    auto moveDownTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveDownPressed);
    if (moveDownTrigger.has_value()) {
        processAction(moveDownTrigger->getAction(), moveDownTrigger->getActionProperties());
    }
    else if (m_map->canSteppedOnTile(Point(m_glPlayer.coord.x(), m_glPlayer.coord.y() + 1))) {
        m_glPlayer.moveDown(tile.getIsWallToClimb());
    } 
    if (tile.getIsWallToClimb()) {
        m_glPlayer.faceUp();
    }
    else {
        m_glPlayer.faceDown();
    }
    setPlayerTexture();
}

void GameWindow::moveLeftPressed() 
{
    //Check if there is an action
    const auto tile = m_map->getTileFromCoord(m_glPlayer.coord);
    auto moveLeftTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveLeftPressed);
    if (moveLeftTrigger.has_value()) {
        processAction(moveLeftTrigger->getAction(), moveLeftTrigger->getActionProperties());
    }
    else if (m_map->canSteppedOnTile(Point(m_glPlayer.coord.x() - 1, m_glPlayer.coord.y()))) {
        m_glPlayer.moveLeft();
    } 
    if (tile.getIsWallToClimb()) {
        m_glPlayer.faceUp();
    }
    else {
        m_glPlayer.faceLeft();
    }
    setPlayerTexture();
}

void GameWindow::moveRightPressed() 
{
    //Check if there is an action
    const auto tile = m_map->getTileFromCoord(m_glPlayer.coord);
    auto moveRightTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveRightPressed);
    if (moveRightTrigger.has_value()) {
        processAction(moveRightTrigger->getAction(), moveRightTrigger->getActionProperties());
    }
    else if (m_map->canSteppedOnTile(Point(m_glPlayer.coord.x() + 1, m_glPlayer.coord.y()))) {
        m_glPlayer.moveRight();
    } 
    if (tile.getIsWallToClimb()) {
        m_glPlayer.faceUp();
    }
    else {
        m_glPlayer.faceRight();
    }
    setPlayerTexture();
}

void GameWindow::calculateTileSize() 
{
    TILEWIDTH = (1.0F / (static_cast<float>(m_width) / 51.2F)) * 2.0F;
    TILEHALFWIDTH = TILEWIDTH / 2.0F;
    TILEHALFHEIGHT = (static_cast<float>(m_width) * TILEHALFWIDTH) / static_cast<float>(m_height);
}

void GameWindow::update(float delta_time) 
{
    if (m_glPlayer.isInMovement()) {
        MovingResult result { m_glPlayer.processMoving(delta_time) };
        if (result.needToRefreshTexture) {
            setPlayerTexture();
        }
        setPlayerPosition();
    }
}

string GameWindow::loadShaderFile(const string &file)
{
    stringstream retVal;
    ifstream shaderFileStream(file, ios::in);
    if (shaderFileStream.is_open()) {
        string line;
        while (getline(shaderFileStream, line)) {
            retVal << line << '\n';
        }
        shaderFileStream.close();
    }
    return retVal.str();
}


void GameWindow::generateGLMapObjects() 
{
    int indexRow {0};
    const Texture *lastUsedTexture { nullptr };
    for(const auto &row : m_map->getTiles()) {
        int indexCol {0};
        for(const auto &tile : row) { 
            GLTile glTile;
            glTile.x = indexCol;
            glTile.y = indexRow;
            glTile.tile = tile;
            GLfloat tileCoord[4][2];
            calculateGLTileCoord(Point(indexCol, indexRow), tileCoord);
            glGenBuffers(1, &glTile.vboPosition);
            glGenBuffers(1, &glTile.vboColor);
            GenerateGLObjectInfo infoGenTexture {
                    lastUsedTexture,
                    &glTile.vao,
                    tile.getTextureName(),
                    tile.getTextureIndex(),
                    &glTile.vboPosition,
                    &glTile.vboColor,
                    &glTile.vboTexture };
            generateGLObject(infoGenTexture, tileCoord, m_texColorBuf);
            if (glTile.tile.hasObjectTexture()) {
                GenerateGLObjectInfo infoGenObject {
                    lastUsedTexture,
                    &glTile.vaoObject,
                    tile.getObjectTextureName(),
                    tile.getObjectTextureIndex(),
                    &glTile.vboPosition,
                    &glTile.vboColor,
                    &glTile.vboTextureObject };
                generateGLObject(infoGenObject, tileCoord, m_texColorBuf);
            }
            indexCol++;
            m_glTiles.push_back(glTile);
        }
        indexRow++;
    }
}

void GameWindow::generateGLObject(GenerateGLObjectInfo &info, const GLfloat tileCoord[4][2], const GLfloat colors[4][3]) 
{
    if (info.lastUsedTexture == nullptr || info.textureName != info.lastUsedTexture->getName()) {
        auto texture { m_map->getTextureByName(info.textureName) };
        if (texture.has_value()) {
            info.lastUsedTexture = &texture.get();
        }
        else {
            info.lastUsedTexture = nullptr;
        }
    }
    if (info.lastUsedTexture != nullptr && !info.textureName.empty() && info.textureIndex != -1) {
        setTextureUVFromIndex(info.lastUsedTexture, m_texCoordBuf, info.textureIndex);
    }
    glBindVertexArray(0);
    glGenVertexArrays(1, info.vao);
    glBindVertexArray(*info.vao);

    /* Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) */
    glBindBuffer(GL_ARRAY_BUFFER, *info.vboPosition);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tileCoord, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    /* Bind our second VBO as being the active buffer and storing vertex attributes (colors) */
    glBindBuffer(GL_ARRAY_BUFFER, *info.vboColor);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    /* Bind our third VBO as being the active buffer and storing vertex attributes (textures) */
    glGenBuffers(1, info.vboTexture);
    glBindBuffer(GL_ARRAY_BUFFER, *info.vboTexture);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), m_texCoordBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void GameWindow::unloadGLMapObjects() 
{
    for(auto &item : m_glTiles) {
        glDeleteBuffers(1, &item.vboPosition); 
        glDeleteBuffers(1, &item.vboColor); 
        glDeleteBuffers(1, &item.vboTexture); 
        glDeleteBuffers(1, &item.vboTextureObject); 
        glDeleteVertexArrays(1, &item.vao);
        if (item.tile.hasObjectTexture()) {
            glDeleteVertexArrays(1, &item.vaoObject);
        }
    }
    m_glTiles.clear();
}

void GameWindow::calculateGLTileCoord(const Point &tilePosition, GLfloat tileCoord[4][2]) 
{
    float startPosX { -1.0F + TILEHALFWIDTH };
    float startPosY { 1.0F - TILEHALFHEIGHT };
    float xConverted { static_cast<float>(tilePosition.x()) };
    float yConverted { static_cast<float>(tilePosition.y()) };

    tileCoord[0][0] = { -TILEHALFWIDTH + startPosX + (TILEWIDTH * xConverted) };                /* Top Left point */
    tileCoord[0][1] = { TILEHALFHEIGHT + startPosY - ((TILEHALFHEIGHT * 2) * yConverted) };     /* Top Left point */
    tileCoord[1][0] = { TILEHALFWIDTH + startPosX + (TILEWIDTH * xConverted) };                 /* Top Right point */
    tileCoord[1][1] = { TILEHALFHEIGHT + startPosY - ((TILEHALFHEIGHT * 2) * yConverted) };     /* Top Right point */
    tileCoord[2][0] = { TILEHALFWIDTH + startPosX + (TILEWIDTH * xConverted) };                 /* Bottom Right point */
    tileCoord[2][1] = { -TILEHALFHEIGHT + startPosY - ((TILEHALFHEIGHT * 2) * yConverted) };    /* Bottom Right point */
    tileCoord[3][0] = { -TILEHALFWIDTH + startPosX + (TILEWIDTH * xConverted) };                /* Bottom Left point */
    tileCoord[3][1] = { -TILEHALFHEIGHT + startPosY - ((TILEHALFHEIGHT * 2) * yConverted) };    /* Bottom Left point */
}

void GameWindow::generateGLPlayerObject() 
{
    float startPosX { -1.0f + TILEHALFWIDTH };
    float startPosY { 1.0f - TILEHALFHEIGHT };

    GLfloat tileCoord[4][2] = {
    { -TILEHALFWIDTH + startPosX,  TILEHALFHEIGHT + startPosY },     /* Top Left point */
    {  TILEHALFWIDTH + startPosX,  TILEHALFHEIGHT + startPosY },     /* Top Right point */
    {  TILEHALFWIDTH + startPosX, -TILEHALFHEIGHT + startPosY },     /* Bottom Right point */
    { -TILEHALFWIDTH + startPosX, -TILEHALFHEIGHT + startPosY } };   /* Bottom Left point */
    
    glGenBuffers(1, &m_glPlayer.vboPosition);
    glGenBuffers(1, &m_glPlayer.vboColor);
    GenerateGLObjectInfo infoGenTexture {
            nullptr,
            &m_glPlayer.vao,
            m_glPlayer.getTextureName(),
            m_glPlayer.getTextureIndex(),
            &m_glPlayer.vboPosition,
            &m_glPlayer.vboColor,
            &m_glPlayer.vboTexture };
    generateGLObject(infoGenTexture, tileCoord, m_texColorBuf);
}

void GameWindow::unloadGLPlayerObject() 
{
    glDeleteBuffers(1, &m_glPlayer.vboPosition); 
    glDeleteBuffers(1, &m_glPlayer.vboColor); 
    glDeleteBuffers(1, &m_glPlayer.vboTexture); 
    glDeleteVertexArrays(1, &m_glPlayer.vao);
}

void GameWindow::render()
{
    m_tileService.useShader();
    m_tileService.setShaderTranslation(m_map->getWidth(), m_map->getHeight(),
                                     m_width, m_height,
                                     m_glPlayer);
    glClearColor(0.3F, 0.3F, 0.3F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    vector<GLTile *> tilesToBeDrawedAfterPlayer;
    for(auto &item : m_glTiles) {
        glBindVertexArray(item.vao);
        glBindBuffer(GL_ARRAY_BUFFER, item.vboPosition);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, item.vboColor);
        glEnableVertexAttribArray(1);
        if (item.tile.hasTexture()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[item.tile.getTextureName()]);
            glBindBuffer(GL_ARRAY_BUFFER, item.vboTexture);
            glEnableVertexAttribArray(2);
        }
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        //Object
        if (item.tile.hasTexture() && item.tile.hasObjectTexture()) {
            if (!item.tile.getObjectAbovePlayer()) {
                drawObjectTile(item);
            }
            else {
                //Add the tile to a list that will be drawed after the player
                tilesToBeDrawedAfterPlayer.push_back(&item);
            }
        }
    }
    //Render the player
    drawPlayer();
    //Draw all the object that appears above the player
    for (auto item : tilesToBeDrawedAfterPlayer) {
        drawObjectTile(*item);
    }
    //Display the FPS
    if (m_toggleFPS) {
        m_textService.useShader();
        m_textService.renderText(m_fpsCalculator.getFPSDisplayText(), 
                               1.0f,                               // X
                               static_cast<float>(m_height) - 24.0f, // Y
                               0.5f,                               // Scale
                               glm::vec3(1.0f, 1.0f, 1.0f));       // Color
    }
    
    // Swap our buffers to make our changes visible 
    SDL_GL_SwapWindow(m_window);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void GameWindow::drawPlayer() 
{
    glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[m_glPlayer.getTextureName()]);
    glBindVertexArray(m_glPlayer.vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_glPlayer.vboPosition);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_glPlayer.vboColor);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_glPlayer.vboTexture);
    glEnableVertexAttribArray(2);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void GameWindow::drawObjectTile(GLTile &tile) 
{
    glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[tile.tile.getObjectTextureName()]);
    glBindVertexArray(tile.vaoObject);
    glBindBuffer(GL_ARRAY_BUFFER, tile.vboPosition);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, tile.vboColor);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, tile.vboTextureObject);
    glEnableVertexAttribArray(2);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void GameWindow::loadMap(const std::string &filePath) 
{
	GameMapStorage mapStorage;
	try {
		mapStorage.loadMap(filePath, m_map);
	}
	catch(invalid_argument &err) {
        cerr << err.what() << '\n';
	}
	catch(runtime_error &err) {
        cerr << err.what() << '\n';
	}
}

void GameWindow::changeMap(const std::string &filePath) 
{
    unloadGLPlayerObject();
    unloadGLMapObjects();
    loadMap(filePath);
    loadTextures();
    generateGLMapObjects();
    generateGLPlayerObject();
    setPlayerPosition();
}

void GameWindow::processAction(MapTileTriggerAction action, const std::map<std::string, std::string> &properties, MapTile *tile, Point tilePosition) 
{
    switch (action) 
    {
        case MapTileTriggerAction::ChangeMap:
            if (properties.at("playerFacing") == "1") {
                m_glPlayer.faceDown();
            }
            m_glPlayer.coord.setX(stoi(properties.at("playerX")));
            m_glPlayer.coord.setY(stoi(properties.at("playerY")));
            changeMap(fmt::format("{0}/maps/{1}", getResourcesPath(), properties.at("mapFileName")));
            break;
        case MapTileTriggerAction::OpenChest:
            if (tile != nullptr) {
                tile->setObjectTextureIndex(stoi(properties.at("objectTextureIndexOpenedChest")));
                //Update the GLTile
                auto iter = find_if(m_glTiles.begin(), m_glTiles.end(), [&tilePosition](GLTile &glTile) {
                    return glTile.x == tilePosition.x() && glTile.y == tilePosition.y();
                });
                if (iter != m_glTiles.end()) {
                    GLTile &glTileToUpdate = *iter;
                    glTileToUpdate.tile = *tile;
                    GLfloat tileCoord[4][2];
                    calculateGLTileCoord(Point(glTileToUpdate.x, glTileToUpdate.y), tileCoord);
                    GenerateGLObjectInfo infoGenObject {
                    nullptr,
                    &glTileToUpdate.vaoObject,
                    tile->getObjectTextureName(),
                    tile->getObjectTextureIndex(),
                    &glTileToUpdate.vboPosition,
                    &glTileToUpdate.vboColor,
                    &glTileToUpdate.vboTextureObject };
                    generateGLObject(infoGenObject, tileCoord, m_texColorBuf);
                }
            }
            break;
        default:   
            break;
    }
}

void GameWindow::loadTextures() 
{
    //Clear existing textures in graphics memory
    for(auto &glTexture : m_texturesGLMap) {
        glDeleteTextures(1, &glTexture.second);
    }
    m_texturesGLMap.clear();
    //Load texture in graphics memory
    for(const auto &texture : m_map->getTextures()) {  
        const auto &textureName { texture.getName() }; 
        glGenTextures(1, &m_texturesGLMap[textureName]);
        glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[textureName]); 
        //texturesObjMap.emplace(textureName, texture);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        string texFileName { texture.getFilename() };
        string fullResourcePath = fmt::format("{0}/{1}", getResourcesPath(), texFileName);
        unsigned char *imageBytes = stbi_load(fullResourcePath.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        if (imageBytes) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageBytes);
        }
        else {
            throw runtime_error(fmt::format("Failed to load texture {0}", fullResourcePath));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(imageBytes);
    }
}

void GameWindow::setTextureUVFromIndex(const Texture *texture, GLfloat uvMap[4][2], int index) 
{
    float indexTile { static_cast<float>(index) };
    const int NBTEXTUREPERLINE { texture->getWidth() / texture->getTileWidth() };
    float lineIndex = floor(indexTile / static_cast<float>(NBTEXTUREPERLINE));
    const float TEXTURETILEWIDTH { texture->getTileWidthGL() };
    const float TEXTURETILEHEIGHT { texture->getTileHeightGL() };
    const float TEXTUREWIDTHADJUSTMENT { TEXTURETILEWIDTH / 40.0f };
    const float TEXTUREHEIGHTADJUSTMENT { TEXTURETILEHEIGHT / 40.0f };
    GLfloat lowerLeftCorner { indexTile / static_cast<float>(NBTEXTUREPERLINE) - floor(indexTile / static_cast<float>(NBTEXTUREPERLINE))  };

    uvMap[0][0] = (lowerLeftCorner) + TEXTUREWIDTHADJUSTMENT;
    uvMap[0][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTURETILEHEIGHT + TEXTUREHEIGHTADJUSTMENT;
    uvMap[1][0] = lowerLeftCorner + TEXTURETILEWIDTH - TEXTUREWIDTHADJUSTMENT;
    uvMap[1][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTURETILEHEIGHT + TEXTUREHEIGHTADJUSTMENT;
    uvMap[2][0] = lowerLeftCorner + TEXTURETILEWIDTH - TEXTUREWIDTHADJUSTMENT;
    uvMap[2][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTUREHEIGHTADJUSTMENT;
    uvMap[3][0] = lowerLeftCorner + TEXTUREWIDTHADJUSTMENT;
    uvMap[3][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTUREHEIGHTADJUSTMENT;
}

void GameWindow::setTileCoordToOrigin() 
{
    float startPosX { -1.0f + TILEHALFWIDTH };
    float startPosY {  1.0f - TILEHALFHEIGHT };
    m_tileCoordBuf[0][0] = -TILEHALFWIDTH + startPosX + TILEWIDTH;
    m_tileCoordBuf[0][1] =  TILEHALFHEIGHT + startPosY - TILEHALFHEIGHT;
    m_tileCoordBuf[1][0] =  TILEHALFWIDTH + startPosX + TILEWIDTH;
    m_tileCoordBuf[1][1] =  TILEHALFHEIGHT + startPosY - TILEHALFHEIGHT;
    m_tileCoordBuf[2][0] =  TILEHALFWIDTH + startPosX + TILEWIDTH;
    m_tileCoordBuf[2][1] = -TILEHALFHEIGHT + startPosY - TILEHALFHEIGHT;
    m_tileCoordBuf[3][0] = -TILEHALFWIDTH + startPosX + TILEWIDTH;
    m_tileCoordBuf[3][1] = -TILEHALFHEIGHT + startPosY - TILEHALFHEIGHT;
}

void GameWindow::setPlayerPosition() 
{
    glBindVertexArray(m_glPlayer.vao);
    setTileCoordToOrigin();
    for(int i = 0; i < 4; i++) {
        m_tileCoordBuf[i][0] += ((static_cast<float>(m_glPlayer.coord.x()) - 1.0f) * TILEWIDTH) + 
                              (m_glPlayer.xMove * TILEWIDTH);
        m_tileCoordBuf[i][1] -= (((static_cast<float>(m_glPlayer.coord.y()) * (TILEHALFHEIGHT * 2.0f)) - TILEHALFHEIGHT)) + 
                              (m_glPlayer.yMove * (TILEHALFHEIGHT * 2.0f));
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_glPlayer.vboPosition);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), m_tileCoordBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

void GameWindow::setPlayerTexture() 
{
    auto texture { m_map->getTextureByName(m_glPlayer.getTextureName()) };
    if (texture.has_value() && !m_glPlayer.getTextureName().empty() && m_glPlayer.getTextureIndex() != -1) {
        setTextureUVFromIndex(texture.get_ptr(), m_texCoordBuf, m_glPlayer.getTextureIndex());
    }
    glBindVertexArray(m_glPlayer.vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_glPlayer.vboTexture);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), m_texCoordBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    glDisableVertexAttribArray(2);
}


const std::string &GameWindow::getExecutablePath() 
{
	if (m_executablePath.empty()) {
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		if (count != -1) {
			m_executablePath = dirname(result);
		}
	}
	return m_executablePath;
}

const std::string &GameWindow::getResourcesPath() 
{
	if (m_resourcesPath.empty()) {
		m_resourcesPath = fmt::format("{0}/resources/", getExecutablePath());
	}
	return m_resourcesPath;
}
