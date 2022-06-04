#include "gameWindow.hpp"
#include "gameMapStorage.hpp"
#include "itemFoundMessageDTO.hpp"
#include <GL/glu.h>
#include <GL/glut.h>
#include <algorithm>
#include <boost/archive/binary_iarchive.hpp>
#include <chrono>
#include <cmath>
#include <fmt/format.h>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <sstream>

using namespace std;

GameWindow::GameWindow(const string &title,
                       int x, int y,
                       int width, int height) 
    : m_WindowSize(width, height),
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
      m_blockKeyDown(false)
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
    if( SDL_GL_SetSwapInterval(-1) < 0 )
    {
        cerr << fmt::format("Warning: Unable to set VSync! SDL Error: {0}\n", SDL_GetError());
        return;
    }
    if (!m_controller.loadItemStore(fmt::format("{0}/items/itemstore.itm", m_controller.getResourcesPath()))) {
        cerr << "Unable to load the item store : " << m_controller.getLastError() << "\n";
        return;
    }
    m_textBox.setItemStore(m_controller.getItemStore());

    if (!m_tileService.initShader(fmt::format("{0}/shaders/tile_330_vs.glsl", m_controller.getResourcesPath()),
                          fmt::format("{0}/shaders/tile_330_fs.glsl", m_controller.getResourcesPath()))) {
        cerr << m_tileService.getLastError() << "\n";
        return;
    }
    if (!m_textBox.initShader(fmt::format("{0}/shaders/textbox_330_vs.glsl", m_controller.getResourcesPath()),
                          fmt::format("{0}/shaders/textbox_330_fs.glsl", m_controller.getResourcesPath()))) {
        cerr << m_textService.getLastError() << "\n";
        return;
    }
    if (!m_textService.initShader(fmt::format("{0}/shaders/text_330_vs.glsl", m_controller.getResourcesPath()),
                          fmt::format("{0}/shaders/text_330_fs.glsl", m_controller.getResourcesPath()))) {
        cerr << m_textService.getLastError() << "\n";
        return;
    }
    if (!m_textService.initFont(fmt::format("{0}/fonts/verdana.ttf", m_controller.getResourcesPath()))) {
        cerr << m_textService.getLastError() << "\n";
        return;
    }
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    m_textService.setProjectionMatrix(projection);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_textureService.setResourcesPath(m_controller.getResourcesPath());
    calculateTileSize();
    m_map = make_shared<GameMap>(1, 1);
    loadMap(fmt::format("{0}/maps/homeHouseV1.map", m_controller.getResourcesPath()));
    loadMapTextures();
    loadItemStoreTextures();
    m_textBox.setScreenSize({ static_cast<float>(m_WindowSize.width()), static_cast<float>(m_WindowSize.height()) });
    m_textBox.setTextService(&m_textService);
    m_textBox.setItemStoreTextureMap(&m_texturesGLItemStore);
    generateGLMapObjects();
    m_glPlayer.initialize();
    m_glPlayer.setTexture({ "playerTexture", "tileNPC1.png", 384, 256, 32, 32 });
    m_textureService.loadTexture(m_glPlayer.getTexture(), m_glPlayer.glTextureId);
    m_glPlayer.generateGLPlayerObject(TILEHALFWIDTH, TILEHALFHEIGHT);
    m_glPlayer.setGLObjectPosition(TILEHALFWIDTH, TILEHALFHEIGHT);

    m_fpsCalculator.initialize();

    SDL_JoystickEventState(SDL_ENABLE);
    m_joystick = SDL_JoystickOpen(0);
}

GameWindow::~GameWindow() 
{
    unloadGLMapObjects();
    m_glPlayer.unloadGLPlayerObject();
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
        if(e.type == SDL_KEYUP) {
            m_blockKeyDown = false;
        }
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
        else if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_RETURN) {
            actionButtonPressed();
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
                actionButtonPressed();
            }
        }
        else if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                int screenWidth = 0;
                int screenHeight = 0;
                SDL_GetWindowSize(m_window, &screenWidth, &screenHeight);
                m_WindowSize.setSize(screenWidth, screenHeight);
                glViewport(0, 0, m_WindowSize.width(), m_WindowSize.height());
                calculateTileSize();
                unloadGLMapObjects();
                m_glPlayer.unloadGLPlayerObject();
                generateGLMapObjects();
                m_glPlayer.generateGLPlayerObject(TILEHALFWIDTH, TILEHALFHEIGHT);
                m_glPlayer.setGLObjectPosition(TILEHALFWIDTH, TILEHALFHEIGHT);

                glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_WindowSize.width()), 0.0f, static_cast<float>(m_WindowSize.height()));
                m_textService.setProjectionMatrix(projection);
                m_textBox.setScreenSize({ static_cast<float>(m_WindowSize.width()), static_cast<float>(m_WindowSize.height()) });
            }
        } 
    }
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    if ((keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL]) && keystate[SDL_SCANCODE_F]) {
        if (!m_blockKeyDown) {
            m_toggleFPS = !m_toggleFPS;
            m_blockKeyDown = true;
        }  
    }
    else if ((keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL]) && keystate[SDL_SCANCODE_M]) {
        //TODO To remove test only
        if (!m_blockKeyDown) {
            m_blockKeyDown = true;
            auto dto = make_unique<ItemFoundMessageDTO>();
            //auto dto = make_unique<MessageDTO>();
            dto->message = "Hello, my name is Jed and I am the first warrior of the entire land so just let me know if you need help.";
            //dto->message = "You found a Wooden Sword!";
            dto->maxDurationInMilliseconds = -1;
            dto->itemId = "swd002";
            dto->textureName = "ItemsTile";
            m_controller.addMessageToPipeline(std::move(dto));
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

void GameWindow::actionButtonPressed()
{
    if (auto currentMessage = m_controller.getCurrentMessage(); currentMessage != nullptr && currentMessage->isDisplayed) {
        m_controller.deleteCurrentMessage();                    
    }
    else {
        //Check if you are facing a tile with a ActionButton trigger configured.
        if (m_glPlayer.isFacing(PlayerFacing::Up)) {
            Point<> tilePositionToProcess = m_glPlayer.getGridPosition();
            tilePositionToProcess.setY(tilePositionToProcess.y() - 1);
            auto &tile = m_map->getTileForEditing(tilePositionToProcess);
            auto actionButtonTrigger = tile.findConstTrigger(MapTileTriggerEvent::ActionButtonPressed);
            if (actionButtonTrigger.has_value()) {
                processAction(actionButtonTrigger->getAction(), actionButtonTrigger->getActionProperties(), &tile, tilePositionToProcess);
            }
        }
    }
}

void GameWindow::moveUpPressed() 
{
    //Check if there is an action
    const auto playerCoord = m_glPlayer.getGridPosition();
    const auto tile = m_map->getTileFromCoord(playerCoord);
    auto moveUpTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveUpPressed);
    if (moveUpTrigger.has_value()) {
        processAction(moveUpTrigger->getAction(), moveUpTrigger->getActionProperties());
    }
    else if (m_map->canSteppedOnTile(Point(playerCoord.x(), playerCoord.y() - 1))) {
        m_glPlayer.moveUp();
    } 
    m_glPlayer.faceUp();
    m_glPlayer.applyCurrentGLTexture(m_textureService);
}

void GameWindow::moveDownPressed() 
{
    //Check if there is an action
    const auto playerCoord = m_glPlayer.getGridPosition();
    const auto tile = m_map->getTileFromCoord(playerCoord);
    auto moveDownTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveDownPressed);
    if (moveDownTrigger.has_value()) {
        processAction(moveDownTrigger->getAction(), moveDownTrigger->getActionProperties());
    }
    else if (m_map->canSteppedOnTile(Point(playerCoord.x(), playerCoord.y() + 1))) {
        m_glPlayer.moveDown(tile.getIsWallToClimb());
    } 
    if (tile.getIsWallToClimb()) {
        m_glPlayer.faceUp();
    }
    else {
        m_glPlayer.faceDown();
    }
    m_glPlayer.applyCurrentGLTexture(m_textureService);
}

void GameWindow::moveLeftPressed() 
{
    //Check if there is an action
    const auto playerCoord = m_glPlayer.getGridPosition();
    const auto tile = m_map->getTileFromCoord(playerCoord);
    auto moveLeftTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveLeftPressed);
    if (moveLeftTrigger.has_value()) {
        processAction(moveLeftTrigger->getAction(), moveLeftTrigger->getActionProperties());
    }
    else if (m_map->canSteppedOnTile(Point(playerCoord.x() - 1, playerCoord.y()))) {
        m_glPlayer.moveLeft();
    } 
    if (tile.getIsWallToClimb()) {
        m_glPlayer.faceUp();
    }
    else {
        m_glPlayer.faceLeft();
    }
    m_glPlayer.applyCurrentGLTexture(m_textureService);
}

void GameWindow::moveRightPressed() 
{
    //Check if there is an action
    const auto playerCoord = m_glPlayer.getGridPosition();
    const auto tile = m_map->getTileFromCoord(playerCoord);
    auto moveRightTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveRightPressed);
    if (moveRightTrigger.has_value()) {
        processAction(moveRightTrigger->getAction(), moveRightTrigger->getActionProperties());
    }
    else if (m_map->canSteppedOnTile(Point(playerCoord.x() + 1, playerCoord.y()))) {
        m_glPlayer.moveRight();
    } 
    if (tile.getIsWallToClimb()) {
        m_glPlayer.faceUp();
    }
    else {
        m_glPlayer.faceRight();
    }
    m_glPlayer.applyCurrentGLTexture(m_textureService);
}

void GameWindow::calculateTileSize() 
{
    Size<float> screenSizeFloat(static_cast<float>(m_WindowSize.width()), static_cast<float>(m_WindowSize.height()));
    TILEWIDTH = (1.0F / (screenSizeFloat.width() / 51.2F)) * 2.0F;
    TILEHALFWIDTH = TILEWIDTH / 2.0F;
    TILEHALFHEIGHT = (screenSizeFloat.width() * TILEHALFWIDTH) / screenSizeFloat.height();
}

void GameWindow::update(float delta_time) 
{
    if (m_glPlayer.isInMovement()) {
        MovingResult result { m_glPlayer.processMoving(delta_time) };
        if (result.needToRefreshTexture) {
            m_glPlayer.applyCurrentGLTexture(m_textureService);
        }
        m_glPlayer.setGLObjectPosition(TILEHALFWIDTH, TILEHALFHEIGHT);
    }
}

void GameWindow::generateGLMapObjects() 
{
    int indexRow {0};
    for(const auto &row : m_map->getTiles()) {
        int indexCol {0};
        for(const auto &tile : row) { 
            GLTile glTile;
            glTile.x = indexCol;
            glTile.y = indexRow;
            glTile.tile = tile;
            GLfloat tileCoord[4][2];
            calculateGLTileCoord(Point(indexCol, indexRow), tileCoord);
            auto tileTexture = m_map->getTextureByName(tile.getTextureName());
            GenerateGLObjectInfo infoGenTexture {
                    &glTile.glObject,
                    tileTexture.has_value() ? &tileTexture.value().get() : nullptr,
                    tile.getTextureIndex()};
            GLObjectService::generateGLObject(infoGenTexture, tileCoord, m_texColorBuf);
            
            if (glTile.tile.hasObjectTexture()) {
                auto objectTexture = m_map->getTextureByName(tile.getObjectTextureName());
                GenerateGLObjectInfo infoGenObject {
                    &glTile.glObject,
                    objectTexture.has_value() ? &objectTexture.value().get() : nullptr,
                    tile.getObjectTextureIndex(),
                    &glTile.vaoObject,
                    &glTile.vboTextureObject };
                GLObjectService::generateGLObject(infoGenObject, tileCoord, m_texColorBuf);
            }
            indexCol++;
            m_glTiles.push_back(glTile);
        }
        indexRow++;
    }
}

void GameWindow::unloadGLMapObjects() 
{
    for(auto &item : m_glTiles) {
        glDeleteBuffers(1, &item.glObject.vboPosition); 
        glDeleteBuffers(1, &item.glObject.vboColor); 
        glDeleteBuffers(1, &item.glObject.vboTexture); 
        glDeleteBuffers(1, &item.vboTextureObject); 
        glDeleteVertexArrays(1, &item.glObject.vao);
        if (item.tile.hasObjectTexture()) {
            glDeleteVertexArrays(1, &item.vaoObject);
        }
    }
    m_glTiles.clear();
}

void GameWindow::calculateGLTileCoord(const Point<> &tilePosition, GLfloat tileCoord[4][2]) 
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

void GameWindow::render()
{
    m_tileService.useShader();
    m_tileService.setShaderTranslation(m_map->getWidth(), m_map->getHeight(),
                                       m_WindowSize.width(), m_WindowSize.height(),
                                       m_glPlayer.getGLObjectPositionWithMovement());
    glClearColor(0.3F, 0.3F, 0.3F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    vector<GLTile *> tilesToBeDrawedAfterPlayer;
    for(auto &item : m_glTiles) {
        glBindVertexArray(item.glObject.vao);
        glBindBuffer(GL_ARRAY_BUFFER, item.glObject.vboPosition);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, item.glObject.vboColor);
        glEnableVertexAttribArray(1);
        if (item.tile.hasTexture()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[item.tile.getTextureName()]);
            glBindBuffer(GL_ARRAY_BUFFER, item.glObject.vboTexture);
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
    m_glPlayer.draw();
    //Draw all the object that appears above the player
    for (auto item : tilesToBeDrawedAfterPlayer) {
        drawObjectTile(*item);
    }
    //Display messages
    auto currentMessage = m_controller.getCurrentMessage();
    if (currentMessage) {
        if (!currentMessage->isDisplayed) {
            m_textBox.generateMessage(currentMessage);
            m_controller.displayCurrentMessage();
        }
        //Display the message
        m_textBox.draw();
        if (currentMessage->isExpired) {
            m_controller.deleteCurrentMessage();
        }
    }
    //Display the FPS
    if (m_toggleFPS) {
        m_textService.useShader();
        m_textService.renderText(m_fpsCalculator.getFPSDisplayText(), 
                               1.0f,                               // X
                               static_cast<float>(m_WindowSize.height()) - 24.0f, // Y
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

void GameWindow::drawObjectTile(GLTile &tile) 
{
    glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[tile.tile.getObjectTextureName()]);
    glBindVertexArray(tile.vaoObject);
    glBindBuffer(GL_ARRAY_BUFFER, tile.glObject.vboPosition);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, tile.glObject.vboColor);
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
    m_glPlayer.unloadGLPlayerObject();
    unloadGLMapObjects();
    loadMap(filePath);
    loadMapTextures();
    generateGLMapObjects();
    m_glPlayer.generateGLPlayerObject(TILEHALFWIDTH, TILEHALFHEIGHT);
    m_glPlayer.setGLObjectPosition(TILEHALFWIDTH, TILEHALFHEIGHT);
}

void GameWindow::processAction(MapTileTriggerAction action, const std::map<std::string, std::string> &properties, MapTile *tile, Point<> tilePosition) 
{
    switch (action) 
    {
        case MapTileTriggerAction::ChangeMap:
            if (properties.at("playerFacing") == "1") {
                m_glPlayer.faceDown();
            }
            m_glPlayer.setGridPosition(Point<>(stoi(properties.at("playerX")), stoi(properties.at("playerY"))));
            changeMap(fmt::format("{0}/maps/{1}", m_controller.getResourcesPath(), properties.at("mapFileName")));
            break;
        case MapTileTriggerAction::OpenChest:
            if (properties.find("itemIdInside") != properties.end()) {
                //Find the item in the item store
                const auto item = m_controller.findItem(properties.find("itemIdInside")->second);
                //Display the item on the screen
                auto msg = std::make_unique<ItemFoundMessageDTO>();
                msg->message = fmt::format("You found a {0}!", item.name);
                msg->maxDurationInMilliseconds = 2000;
                msg->itemId = item.id;
                msg->textureName = item.textureName;
                m_controller.addMessageToPipeline(std::move(msg));
            }
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
                    auto newChestTexture = m_map->getTextureByName(tile->getObjectTextureName());
                    GenerateGLObjectInfo infoGenObject {
                        &glTileToUpdate.glObject,
                        newChestTexture.has_value() ? &newChestTexture.value().get() : nullptr,
                        tile->getObjectTextureIndex(),
                        &glTileToUpdate.vaoObject,
                        &glTileToUpdate.vboTextureObject 
                    };
                    GLObjectService::generateGLObject(infoGenObject, tileCoord, m_texColorBuf);
                }
            }
            break;
        default:   
            break;
    }
}

void GameWindow::loadMapTextures() 
{
    //Clear existing textures in graphics memory
    for(auto &glTexture : m_texturesGLMap) {
        m_textureService.unloadTexture(glTexture.second);
    }
    m_texturesGLMap.clear();
    //Load texture in graphics memory
    for(const auto &texture : m_map->getTextures()) {  
        const auto &textureName { texture.getName() }; 
        m_textureService.loadTexture(texture, m_texturesGLMap[textureName]);
    }
}
void GameWindow::loadItemStoreTextures()
{
    //Clear existing textures in graphics memory
    for(auto &glTexture : m_texturesGLItemStore) {
        glDeleteTextures(1, &glTexture.second);
    }
    m_texturesGLItemStore.clear();
    for(const auto &texture : m_controller.getItemStore()->getTextureContainer().getTextures()) {
        const auto &textureName { texture.getName() }; 
        m_textureService.loadTexture(texture, m_texturesGLItemStore[textureName]);
    }
}