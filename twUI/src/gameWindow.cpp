#include "gameWindow.hpp"
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <fmt/format.h>
#include <fstream>
#include <sstream>
#include <boost/archive/binary_iarchive.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

GameWindow::GameWindow(const string &title,
                       int x, int y,
                       int width, int height) 
    : width(width),
      height(height),
      mustExit(false),
      texCoordBuf { { 0.0f, 0.0f },
                    { 0.0f, 0.0f }, 
                    { 0.0f, 0.0f }, 
                    { 0.0f, 0.0f } },
      texColorBuf { { 1.0f, 1.0f, 1.0f },   /* Red */
                    { 1.0f, 1.0f, 1.0f },   /* Green */
                    { 1.0f, 1.0f, 1.0f },   /* Blue */
                    { 1.0f, 1.0f, 1.0f } },
      toggleFPS(false),
      blockToggleFPS(false)
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
    window = SDL_CreateWindow(title.c_str(), 
        x, 
        y, 
        width, 
        height, 
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED);
    if(window == NULL)
    {
        cerr << fmt::format("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    //Create context
    gContext = SDL_GL_CreateContext(window);
    if(gContext == nullptr)
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

    if (!tileService.initShader(fmt::format("{0}/shaders/tile_330_vs.glsl", getResourcesPath()),
                          fmt::format("{0}/shaders/tile_330_fs.glsl", getResourcesPath()))) {
        cerr << tileService.getLastError() << "\n";
        return;
    }
    if (!textService.initShader(fmt::format("{0}/shaders/text_330_vs.glsl", getResourcesPath()),
                          fmt::format("{0}/shaders/text_330_fs.glsl", getResourcesPath()))) {
        cerr << textService.getLastError() << "\n";
        return;
    }
    if (!textService.initFont(fmt::format("{0}/verdana.ttf", getResourcesPath()))) {
        cerr << textService.getLastError() << "\n";
        return;
    }
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    textService.setProjectionMatrix(projection);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    calculateTileSize();
    loadMap(fmt::format("{0}/maps/homeHouseV1.map", getResourcesPath()));
    loadTextures();
    generateGLMapObjects();
    glPlayer.initialize();
    generateGLPlayerObject();
    setPlayerPosition();

    fpsCalculator.initialize();

    SDL_JoystickEventState(SDL_DISABLE);
    joystick = SDL_JoystickOpen(0);
}

GameWindow::~GameWindow() 
{
    unloadGLMapObjects();
    unloadGLPlayerObject();
    SDL_JoystickClose(joystick);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GameWindow::show() 
{
    SDL_ShowWindow(window);
}

void GameWindow::hide() 
{
    SDL_HideWindow(window);
}

bool GameWindow::isAlive() const
{
    return !mustExit;
}

void GameWindow::processEvents() 
{
    SDL_Event e;
    SDL_JoystickUpdate();
    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT){
            mustExit = true;
        }
        else if(e.type == SDL_KEYDOWN && !glPlayer.isInMovement()) {
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
            if (blockToggleFPS) {
                blockToggleFPS = false;
            }
        }
        else if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_GetWindowSize(window, &width, &height);
                glViewport(0,0,width,height);
                calculateTileSize();
                unloadGLMapObjects();
                unloadGLPlayerObject();
                generateGLMapObjects();
                generateGLPlayerObject();
                setPlayerPosition();

                glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
                textService.setProjectionMatrix(projection);
            }
        } 
    }
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    if ((keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL]) && keystate[SDL_SCANCODE_F]) {
        if (!blockToggleFPS) {
            toggleFPS = !toggleFPS;
            blockToggleFPS = true;
        }
    }
    for (int i = 0 ; i < SDL_JoystickNumHats(joystick) ; i++ ) {
        if (SDL_JoystickGetHat(joystick, i) == SDL_HAT_UP && !glPlayer.isInMovement()) {
            moveUpPressed();
            break;
        }
        else if (SDL_JoystickGetHat(joystick, i) == SDL_HAT_DOWN && !glPlayer.isInMovement()) {
            moveDownPressed();
            break;
        }
        else if (SDL_JoystickGetHat(joystick, i) == SDL_HAT_LEFT && !glPlayer.isInMovement()) {
            moveLeftPressed();
            break;
        }
        else if (SDL_JoystickGetHat(joystick, i) == SDL_HAT_RIGHT && !glPlayer.isInMovement()) {
            moveRightPressed();
            break;
        }
    }
    update(1.0f / 90.0f);
	render();
    if (toggleFPS) {
        fpsCalculator.calculate();
    }
	//SDL_GL_SwapWindow(window);
}

void GameWindow::moveUpPressed() 
{
    //Check if there is an action
    const auto tile = map->getTileFromCoord(glPlayer.coord);
    if (tile.getTrigger() == MapTileTriggerEvent::MoveUpPressed) {
        processAction(tile.getAction(), tile.getActionProperties());
        return;
    }
    if (map->canSteppedOnTile(Point(glPlayer.coord.x(), glPlayer.coord.y() - 1))) {
        glPlayer.moveUp();
    } 
    else {
        glPlayer.faceUp();
    }
    setPlayerTexture();
}

void GameWindow::moveDownPressed() 
{
    //Check if there is an action
    const auto tile = map->getTileFromCoord(glPlayer.coord);
    if (tile.getTrigger() == MapTileTriggerEvent::MoveDownPressed) {
        processAction(tile.getAction(), tile.getActionProperties());
        return;
    }
    if (map->canSteppedOnTile(Point(glPlayer.coord.x(), glPlayer.coord.y() + 1))) {
        glPlayer.moveDown();
    } 
    else {
        glPlayer.faceDown();
    }
    setPlayerTexture();
}

void GameWindow::moveLeftPressed() 
{
    //Check if there is an action
    const auto tile = map->getTileFromCoord(glPlayer.coord);
    if (tile.getTrigger() == MapTileTriggerEvent::MoveLeftPressed) {
        processAction(tile.getAction(), tile.getActionProperties());
        return;
    }
    if (map->canSteppedOnTile(Point(glPlayer.coord.x() - 1, glPlayer.coord.y()))) {
        glPlayer.moveLeft();
    } 
    else {
        glPlayer.faceLeft();
    }
    setPlayerTexture();
}

void GameWindow::moveRightPressed() 
{
    //Check if there is an action
    const auto tile = map->getTileFromCoord(glPlayer.coord);
    if (tile.getTrigger() == MapTileTriggerEvent::MoveRightPressed) {
        processAction(tile.getAction(), tile.getActionProperties());
        return;
    }
    if (map->canSteppedOnTile(Point(glPlayer.coord.x() + 1, glPlayer.coord.y()))) {
        glPlayer.moveRight();
    } 
    else {
        glPlayer.faceRight();
    }
    setPlayerTexture();
}

void GameWindow::calculateTileSize() 
{
    TILEWIDTH = (1.0f / (static_cast<float>(width) / 51.2f)) * 2.0f;
    TILEHALFWIDTH = TILEWIDTH / 2.0f;
    TILEHALFHEIGHT = (static_cast<float>(width) * TILEHALFWIDTH) / static_cast<float>(height);
}

void GameWindow::update(double delta_time) 
{
    if (glPlayer.isInMovement()) {
        MovingResult result { glPlayer.processMoving(delta_time) };
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
    float startPosX { -1.0f + TILEHALFWIDTH };
    float startPosY { 1.0f - TILEHALFHEIGHT };

    GLfloat tileCoord[4][2] = {
    { -TILEHALFWIDTH + startPosX,  TILEHALFHEIGHT + startPosY },     /* Top Left point */
    {  TILEHALFWIDTH + startPosX,  TILEHALFHEIGHT + startPosY },     /* Top Right point */
    {  TILEHALFWIDTH + startPosX, -TILEHALFHEIGHT + startPosY },     /* Bottom Right point */
    { -TILEHALFWIDTH + startPosX, -TILEHALFHEIGHT + startPosY } };   /* Bottom Left point */
    
    int indexRow {0};
    const Texture *lastUsedTexture { nullptr };
    for(const auto &row : map->getTiles()) {
        int indexCol {0};
        for(const auto &tile : row) { 
            GLTile glTile;
            glTile.x = indexCol;
            glTile.y = indexRow;
            glTile.tile = tile;
            /*glTile.hasTexture = !tile.getTextureName().empty() && tile.getTextureIndex() != -1;
            if (glTile.hasTexture) {
                glTile.textureName = tile.getTextureName();
                glTile.textureIndex = tile.getTextureIndex();
            }
            glTile.hasObjectTexture = !tile.getObjectTextureName().empty() && tile.getObjectTextureIndex() != -1;
            if (glTile.hasObjectTexture) {
                glTile.objectTextureName = tile.getObjectTextureName();
                glTile.objectTextureIndex = tile.getObjectTextureIndex();
            }
            glTile.objectAbovePlayer = tile.getObjectAbovePlayer();*/
            if (indexCol > 0) {
                for (int indexVertex=0; indexVertex<4; indexVertex++) {
                    tileCoord[indexVertex][0] += TILEWIDTH;
                }
            }
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
            generateGLObject(infoGenTexture, tileCoord, texColorBuf);
            if (glTile.tile.hasObjectTexture()) {
                GenerateGLObjectInfo infoGenObject {
                    lastUsedTexture,
                    &glTile.vaoObject,
                    tile.getObjectTextureName(),
                    tile.getObjectTextureIndex(),
                    &glTile.vboPosition,
                    &glTile.vboColor,
                    &glTile.vboTextureObject };
                generateGLObject(infoGenObject, tileCoord, texColorBuf);
            }
            indexCol++;
            glTiles.push_back(glTile);
        }
        indexRow++;
        float topOffset { static_cast<float>(indexRow) * (TILEHALFHEIGHT * 2.0f) };
        tileCoord[0][0] = -TILEHALFWIDTH + startPosX;
        tileCoord[0][1] =  TILEHALFHEIGHT + startPosY - topOffset;
        tileCoord[1][0] =  TILEHALFWIDTH + startPosX;
        tileCoord[1][1] =  TILEHALFHEIGHT + startPosY - topOffset;
        tileCoord[2][0] =  TILEHALFWIDTH + startPosX;
        tileCoord[2][1] = -TILEHALFHEIGHT + startPosY - topOffset;
        tileCoord[3][0] = -TILEHALFWIDTH + startPosX;
        tileCoord[3][1] = -TILEHALFHEIGHT + startPosY - topOffset;
    }
}

void GameWindow::generateGLObject(GenerateGLObjectInfo &info, const GLfloat tileCoord[4][2], const GLfloat colors[4][3]) 
{
    if (info.lastUsedTexture == nullptr || info.textureName != info.lastUsedTexture->getName()) {
        auto texture { map->getTextureByName(info.textureName) };
        if (texture.has_value()) {
            info.lastUsedTexture = &texture.get();
        }
        else {
            info.lastUsedTexture = nullptr;
        }
    }
    if (info.lastUsedTexture != nullptr && !info.textureName.empty() && info.textureIndex != -1) {
        setTextureUVFromIndex(info.lastUsedTexture, texCoordBuf, info.textureIndex);
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
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), texCoordBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void GameWindow::unloadGLMapObjects() 
{
    for(auto &item : glTiles) {
        glDeleteBuffers(1, &item.vboPosition); 
        glDeleteBuffers(1, &item.vboColor); 
        glDeleteBuffers(1, &item.vboTexture); 
        glDeleteBuffers(1, &item.vboTextureObject); 
        glDeleteVertexArrays(1, &item.vao);
        if (item.tile.hasObjectTexture()) {
            glDeleteVertexArrays(1, &item.vaoObject);
        }
    }
    glTiles.clear();
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
    
    glGenBuffers(1, &glPlayer.vboPosition);
    glGenBuffers(1, &glPlayer.vboColor);
    GenerateGLObjectInfo infoGenTexture {
            nullptr,
            &glPlayer.vao,
            glPlayer.getTextureName(),
            glPlayer.getTextureIndex(),
            &glPlayer.vboPosition,
            &glPlayer.vboColor,
            &glPlayer.vboTexture };
    generateGLObject(infoGenTexture, tileCoord, texColorBuf);
}

void GameWindow::unloadGLPlayerObject() 
{
    glDeleteBuffers(1, &glPlayer.vboPosition); 
    glDeleteBuffers(1, &glPlayer.vboColor); 
    glDeleteBuffers(1, &glPlayer.vboTexture); 
    glDeleteVertexArrays(1, &glPlayer.vao);
}

void GameWindow::render()
{
    tileService.useShader();
    tileService.setShaderTranslation(map->getWidth(), map->getHeight(),
                                     width, height,
                                     glPlayer);
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    vector<GLTile *> tilesToBeDrawedAfterPlayer;
    for(auto &item : glTiles) {
        glBindVertexArray(item.vao);
        glBindBuffer(GL_ARRAY_BUFFER, item.vboPosition);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, item.vboColor);
        glEnableVertexAttribArray(1);
        if (item.tile.hasTexture()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texturesGLMap[item.tile.getTextureName()]);
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
    if (toggleFPS) {
        textService.useShader();
        textService.renderText(fpsCalculator.getFPSDisplayText(), 
                               1.0f,                               // X
                               static_cast<float>(height) - 24.0f, // Y
                               0.5f,                               // Scale
                               glm::vec3(1.0f, 1.0f, 1.0f));       // Color
    }
    
    // Swap our buffers to make our changes visible 
    SDL_GL_SwapWindow(window);
    
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
    glBindTexture(GL_TEXTURE_2D, texturesGLMap[glPlayer.getTextureName()]);
    glBindVertexArray(glPlayer.vao);
    glBindBuffer(GL_ARRAY_BUFFER, glPlayer.vboPosition);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, glPlayer.vboColor);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, glPlayer.vboTexture);
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
    glBindTexture(GL_TEXTURE_2D, texturesGLMap[tile.tile.getObjectTextureName()]);
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
    map.reset();
    ifstream ofs(filePath, ifstream::binary);
	boost::archive::binary_iarchive oa(ofs);
    map = make_shared<GameMap>(1, 1);
	oa >> *map;
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

void GameWindow::processAction(MapTileTriggerAction action, const std::map<std::string, std::string> &properties) 
{
    if (action == MapTileTriggerAction::ChangeMap) {
        if (properties.at("playerFacing") == "1") {
            glPlayer.faceDown();
        }
        glPlayer.coord.setX(stoi(properties.at("playerX")));
        glPlayer.coord.setY(stoi(properties.at("playerY")));
        changeMap(fmt::format("{0}/maps/{1}", getResourcesPath(), properties.at("mapFileName")));
        return;
    }
}


void GameWindow::loadTextures() 
{
    //Clear existing textures in graphics memory
    for(auto &glTexture : texturesGLMap) {
        glDeleteTextures(1, &glTexture.second);
    }
    texturesGLMap.clear();
    //Load texture in graphics memory
    for(const auto &texture : map->getTextures()) {  
        const auto &textureName { texture.getName() }; 
        glGenTextures(1, &texturesGLMap[textureName]);
        glBindTexture(GL_TEXTURE_2D, texturesGLMap[textureName]); 
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
    float lineIndex = static_cast<int>(indexTile / NBTEXTUREPERLINE);
    const float TEXTURETILEWIDTH { texture->getTileWidthGL() };
    const float TEXTURETILEHEIGHT { texture->getTileHeightGL() };
    GLfloat lowerLeftCorner { indexTile / static_cast<float>(NBTEXTUREPERLINE) - floor(indexTile / static_cast<float>(NBTEXTUREPERLINE))  };

    uvMap[0][0] = (lowerLeftCorner);
    uvMap[0][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTURETILEHEIGHT;
    uvMap[1][0] = lowerLeftCorner + TEXTURETILEWIDTH;
    uvMap[1][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTURETILEHEIGHT;
    uvMap[2][0] = lowerLeftCorner + TEXTURETILEWIDTH;;
    uvMap[2][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex);
    uvMap[3][0] = lowerLeftCorner;
    uvMap[3][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex);
}

void GameWindow::setTileCoordToOrigin() 
{
    float startPosX { -1.0f + TILEHALFWIDTH };
    float startPosY {  1.0f - TILEHALFHEIGHT };
    tileCoordBuf[0][0] = -TILEHALFWIDTH + startPosX + TILEWIDTH;
    tileCoordBuf[0][1] =  TILEHALFHEIGHT + startPosY - TILEHALFHEIGHT;
    tileCoordBuf[1][0] =  TILEHALFWIDTH + startPosX + TILEWIDTH;
    tileCoordBuf[1][1] =  TILEHALFHEIGHT + startPosY - TILEHALFHEIGHT;
    tileCoordBuf[2][0] =  TILEHALFWIDTH + startPosX + TILEWIDTH;
    tileCoordBuf[2][1] = -TILEHALFHEIGHT + startPosY - TILEHALFHEIGHT;
    tileCoordBuf[3][0] = -TILEHALFWIDTH + startPosX + TILEWIDTH;
    tileCoordBuf[3][1] = -TILEHALFHEIGHT + startPosY - TILEHALFHEIGHT;
}

void GameWindow::setPlayerPosition() 
{
    glBindVertexArray(glPlayer.vao);
    setTileCoordToOrigin();
    for(int i = 0; i < 4; i++) {
        tileCoordBuf[i][0] += ((static_cast<float>(glPlayer.coord.x()) - 1.0f) * TILEWIDTH) + 
                              (glPlayer.xMove * TILEWIDTH);
        tileCoordBuf[i][1] -= (((static_cast<float>(glPlayer.coord.y()) * (TILEHALFHEIGHT * 2.0f)) - TILEHALFHEIGHT)) + 
                              (glPlayer.yMove * (TILEHALFHEIGHT * 2.0f));
    }
    glBindBuffer(GL_ARRAY_BUFFER, glPlayer.vboPosition);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tileCoordBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

void GameWindow::setPlayerTexture() 
{
    auto texture { map->getTextureByName(glPlayer.getTextureName()) };
    if (texture.has_value() && !glPlayer.getTextureName().empty() && glPlayer.getTextureIndex() != -1) {
        setTextureUVFromIndex(texture.get_ptr(), texCoordBuf, glPlayer.getTextureIndex());
    }
    glBindVertexArray(glPlayer.vao);

    glBindBuffer(GL_ARRAY_BUFFER, glPlayer.vboTexture);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), texCoordBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    glDisableVertexAttribArray(2);
}


const std::string &GameWindow::getExecutablePath() 
{
	if (executablePath.empty()) {
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		if (count != -1) {
			executablePath = dirname(result);
		}
	}
	return executablePath;
}

const std::string &GameWindow::getResourcesPath() 
{
	if (resourcesPath.empty()) {
		resourcesPath = fmt::format("{0}/resources/", getExecutablePath());
	}
	return resourcesPath;
}
