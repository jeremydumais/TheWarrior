#include "gameMapMode.hpp"
#include "gameMap.hpp"
#include "gameMapStorage.hpp"
#include "itemFoundMessageDTO.hpp"
#include "fmt/format.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

GameMapMode::GameMapMode()
{
    m_choicePopup.m_choiceClicked.connect(boost::bind(&GameMapMode::mainMenuPopupClicked, this, boost::placeholders::_1));
    m_choicePopup.m_cancelClicked.connect(boost::bind(&GameMapMode::mainMenuPopupCanceled, this));    
}

void GameMapMode::initialize(const std::string &resourcesPath,
                             std::shared_ptr<GLPlayer> glPlayer,
                             std::shared_ptr<ItemStore> itemStore, 
                             std::shared_ptr<MessagePipeline> messagePipeline,
                             std::shared_ptr<GLTileService> tileService,
                             std::shared_ptr<GLTextBox> textBox,
                             std::shared_ptr<GLTextService> textService,
                             const std::map<std::string, unsigned int> *texturesGLItemStore,
                             std::shared_ptr<InputDevicesState> inputDevicesState)
{
    m_resourcesPath = resourcesPath;
    m_map = std::make_shared<GameMap>(1, 1);
    m_glPlayer = glPlayer;
    m_glFormService->initialize(m_shaderProgram, textService);
    m_glCharacterWindow.initialize(resourcesPath, glPlayer, textService, itemStore, texturesGLItemStore, inputDevicesState);
    m_glInventory.initialize(resourcesPath, glPlayer, textService, itemStore, texturesGLItemStore, inputDevicesState);
    m_glInventory.setInventory(m_glPlayer->getInventory());
    m_textureService.setResourcesPath(resourcesPath);
    m_tileService = tileService;
    m_textBox = textBox;
    m_inputDevicesState = inputDevicesState;
    m_controller.initialize(itemStore, messagePipeline);
    m_choicePopup.initialize(resourcesPath, m_glFormService, textService, inputDevicesState);
    loadMap(fmt::format("{0}/maps/homeHouseV1.map", resourcesPath), "homeHouseV1.map");
    loadMapTextures();
    generateGLMapObjects();
    m_glCharacterWindow.onCloseEvent.connect(boost::bind(&GameMapMode::onCharacterWindowClose, this));
    m_glInventory.onCloseEvent.connect(boost::bind(&GameMapMode::onInventoryWindowClose, this));
}

bool GameMapMode::initShaders(const std::string &resourcesPath)
{
    m_shaderProgram = std::make_shared<GLShaderProgram>(fmt::format("{0}/shaders/window_330_vs.glsl", resourcesPath),
                                                        fmt::format("{0}/shaders/window_330_fs.glsl", resourcesPath));
    if (!m_shaderProgram->compileShaders()) {
        m_lastError = m_shaderProgram->getLastError();
        return false;
    }
    if (!m_shaderProgram->linkShaders({ "vertex" })) {
        m_lastError = m_shaderProgram->getLastError();
        return false;
    }
    m_glCharacterWindow.initShader(m_shaderProgram);
    m_glInventory.initShader(m_shaderProgram);
    return true;
}

const std::string& GameMapMode::getLastError() const
{
    return m_lastError;
}

void GameMapMode::processEvents(SDL_Event &e)
{
    if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_i) {
        toggleInventoryWindow();
    }
    else if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_c) {
        toggleCharacterWindow();
    }
    else if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE) {
        if (m_inputMode == GameMapInputMode::Map) {
            showMainMenu();
            m_inputDevicesState->reset();
        }
    }
}

void GameMapMode::update()
{
    switch (m_inputMode) {
        case GameMapInputMode::Map:
            if(m_inputDevicesState->isADirectionKeyPressed()) {
                if (!m_glPlayer->isInMovement()) {
                    if (m_inputDevicesState->getUpPressed()) {
                        moveUpPressed();
                    }
                    else if (m_inputDevicesState->getDownPressed()) {
                        moveDownPressed();
                    }
                    else if (m_inputDevicesState->getLeftPressed()) {
                        moveLeftPressed();
                    }
                    else if (m_inputDevicesState->getRightPressed()) {
                        moveRightPressed();
                    }
                }
            }
            if(m_inputDevicesState->getButtonAState() == InputElementState::Released) {
                actionButtonPressed();
            }
            if(m_inputDevicesState->isADirectionKeyPressed()) {
                if (m_inputDevicesState->getButtonBState() == InputElementState::Pressed ||
                    m_inputDevicesState->getKeyShiftState() == InputElementState::Pressed) {
                    m_glPlayer->enableRunMode();
                }
                else {
                    m_glPlayer->disableRunMode();
                }
            } 
            if(m_inputDevicesState->getButtonCState() == InputElementState::Released) {
                showMainMenu();
            }
            break;
        case GameMapInputMode::MainMenuPopup:
            m_choicePopup.update();
            break;
        case GameMapInputMode::InventoryWindow:
            m_glInventory.update();
            break;
        case GameMapInputMode::CharacterWindow:
            m_glCharacterWindow.update();
            break;
    }
}

void GameMapMode::gameWindowSizeChanged(const Size<> &size)
{
    m_screenSize = size;
    m_glFormService->gameWindowSizeChanged(size);
    unloadGLMapObjects();
    generateGLMapObjects();
    m_glInventory.gameWindowSizeChanged(size);
    m_glCharacterWindow.gameWindowSizeChanged(size);
    m_choicePopup.gameWindowLocationChanged({static_cast<float>(size.width()) / 2.0F,
                                             static_cast<float>(size.height()) / 2.0F});
}

void GameMapMode::gameWindowTileSizeChanged(const TileSize &tileSize)
{
    m_tileSize = tileSize;
}

void GameMapMode::showMainMenu()
{
    m_inputMode = GameMapInputMode::MainMenuPopup;
    m_choicePopup.preparePopup({"Inventory", "Character", "Back"});
    m_choicePopup.generateGLElements();
}

void GameMapMode::toggleInventoryWindow()
{
    if (m_inputMode == GameMapInputMode::Map || m_inputMode == GameMapInputMode::MainMenuPopup) {
        m_inputMode = GameMapInputMode::InventoryWindow;
        m_glInventory.generateGLInventory();
    }
    else if (m_inputMode == GameMapInputMode::InventoryWindow) {
        m_inputMode = GameMapInputMode::Map;
        onInventoryWindowClose();
    }
}

void GameMapMode::toggleCharacterWindow()
{
    if (m_inputMode == GameMapInputMode::Map || m_inputMode == GameMapInputMode::MainMenuPopup) {
        m_inputMode = GameMapInputMode::CharacterWindow;
        m_glCharacterWindow.generateGLElements();
    }
    else if (m_inputMode == GameMapInputMode::CharacterWindow) {
        m_inputMode = GameMapInputMode::Map;
        onCharacterWindowClose();
    }
}

void GameMapMode::render()
{
    m_tileService->useShader();
    m_tileService->setShaderTranslation(m_map->getWidth(), m_map->getHeight(),
                                       m_screenSize.width(), m_screenSize.height(),
                                       m_glPlayer->getGLObjectPositionWithMovement());
    glClearColor(0.3F, 0.3F, 0.3F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    std::vector<GLTile *> tilesToBeDrawedAfterPlayer;
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
    m_glPlayer->draw();
    //Draw all the object that appears above the player
    for (auto item : tilesToBeDrawedAfterPlayer) {
        drawObjectTile(*item);
    }
    //Display messages
    auto currentMessage = m_controller.getCurrentMessage();
    if (currentMessage) {
        if (!currentMessage->isDisplayed) {
            m_textBox->generateMessage(currentMessage);
            m_controller.displayCurrentMessage();
        }
        //Display the message
        m_textBox->draw();
        if (currentMessage->isExpired) {
            m_controller.deleteCurrentMessage();
        }
    }
    if (m_inputMode == GameMapInputMode::MainMenuPopup) {
        m_choicePopup.render();
    }
    if (m_inputMode == GameMapInputMode::InventoryWindow) {
        m_glInventory.render();
    }
    if (m_inputMode == GameMapInputMode::CharacterWindow) {
        m_glCharacterWindow.render();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void GameMapMode::drawObjectTile(GLTile &tile) 
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

void GameMapMode::actionButtonPressed()
{
    if (m_controller.isMessageDisplayed()) {
        m_controller.acknowledgeMessage();                    
    }
    else {
        //Check if you are facing a tile with a ActionButton trigger configured.
        if (m_glPlayer->isFacing(PlayerFacing::Up)) {
            Point<> tilePositionToProcess = m_glPlayer->getGridPosition();
            tilePositionToProcess.setY(tilePositionToProcess.y() - 1);
            auto &tile = m_map->getTileForEditing(tilePositionToProcess);
            auto actionButtonTrigger = tile.findConstTrigger(MapTileTriggerEvent::ActionButtonPressed);
            if (actionButtonTrigger.has_value()) {
                processAction(actionButtonTrigger->getAction(), actionButtonTrigger->getActionProperties(), &tile, tilePositionToProcess);
            }
        }
    }
}

void GameMapMode::moveUpPressed() 
{
    //Check if there is an action
    const auto playerCoord = m_glPlayer->getGridPosition();
    const auto tile = m_map->getTileFromCoord(playerCoord);
    auto moveUpTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveUpPressed);
    if (moveUpTrigger.has_value()) {
        processAction(moveUpTrigger->getAction(), moveUpTrigger->getActionProperties());
    }
    else if (m_map->canSteppedOnTile(Point(playerCoord.x(), playerCoord.y() - 1))) {
        m_glPlayer->moveUp();
    } 
    m_glPlayer->faceUp();
    m_glPlayer->applyCurrentGLTexture(m_textureService);
}

void GameMapMode::moveDownPressed() 
{
    //Check if there is an action
    const auto playerCoord = m_glPlayer->getGridPosition();
    const auto tile = m_map->getTileFromCoord(playerCoord);
    auto moveDownTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveDownPressed);
    if (moveDownTrigger.has_value()) {
        processAction(moveDownTrigger->getAction(), moveDownTrigger->getActionProperties());
    }
    else if (m_map->canSteppedOnTile(Point(playerCoord.x(), playerCoord.y() + 1))) {
        m_glPlayer->moveDown(tile.getIsWallToClimb());
    } 
    if (tile.getIsWallToClimb()) {
        m_glPlayer->faceUp();
    }
    else {
        m_glPlayer->faceDown();
    }
    m_glPlayer->applyCurrentGLTexture(m_textureService);
}

void GameMapMode::moveLeftPressed() 
{
    //Check if there is an action
    const auto playerCoord = m_glPlayer->getGridPosition();
    const auto tile = m_map->getTileFromCoord(playerCoord);
    auto moveLeftTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveLeftPressed);
    if (moveLeftTrigger.has_value()) {
        processAction(moveLeftTrigger->getAction(), moveLeftTrigger->getActionProperties());
    }
    else if (m_map->canSteppedOnTile(Point(playerCoord.x() - 1, playerCoord.y()))) {
        m_glPlayer->moveLeft();
    } 
    if (tile.getIsWallToClimb()) {
        m_glPlayer->faceUp();
    }
    else {
        m_glPlayer->faceLeft();
    }
    m_glPlayer->applyCurrentGLTexture(m_textureService);
}

void GameMapMode::moveRightPressed() 
{
    //Check if there is an action
    const auto playerCoord = m_glPlayer->getGridPosition();
    const auto tile = m_map->getTileFromCoord(playerCoord);
    auto moveRightTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveRightPressed);
    if (moveRightTrigger.has_value()) {
        processAction(moveRightTrigger->getAction(), moveRightTrigger->getActionProperties());
    }
    else if (m_map->canSteppedOnTile(Point(playerCoord.x() + 1, playerCoord.y()))) {
        m_glPlayer->moveRight();
    } 
    if (tile.getIsWallToClimb()) {
        m_glPlayer->faceUp();
    }
    else {
        m_glPlayer->faceRight();
    }
    m_glPlayer->applyCurrentGLTexture(m_textureService);
}

void GameMapMode::processAction(MapTileTriggerAction action, const std::map<std::string, std::string> &properties, MapTile *tile, Point<> tilePosition) 
{
    switch (action) 
    {
        case MapTileTriggerAction::ChangeMap:
            if (properties.at("playerFacing") == "1") {
                m_glPlayer->faceDown();
            }
            m_glPlayer->setGridPosition(Point<>(stoi(properties.at("playerX")), stoi(properties.at("playerY"))));
            changeMap(fmt::format("{0}/maps/{1}", m_resourcesPath, properties.at("mapFileName")), properties.at("mapFileName"));
            break;
        case MapTileTriggerAction::OpenChest:
            {
                //Check if the item has already been taken
                auto tileIndex = m_map->getTileIndexFromCoord(tilePosition);
                if (!m_controller.isTileActionAlreadyProcessed(m_currentMapName, tileIndex)) {
                    if (properties.find("itemIdInside") != properties.end()) {
                        auto itemIdInside = properties.find("itemIdInside")->second;
                        //Find the item in the item store
                        const auto item = m_controller.findItem(itemIdInside);
                        m_controller.addItemToInventory(dynamic_cast<Player *>(m_glPlayer.get()), itemIdInside);
                        //Display the item on the screen
                        auto msg = std::make_unique<ItemFoundMessageDTO>();
                        msg->message = fmt::format("You found a {0}!", item.name);
                        msg->maxDurationInMilliseconds = 2000;
                        msg->itemId = item.id;
                        msg->textureName = item.textureName;
                        m_controller.addMessageToPipeline(std::move(msg));
                    }
                    m_controller.addTileActionProcessed(m_currentMapName, tileIndex);
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
            }
            break;
        default:   
            break;
    }
}

void GameMapMode::loadMap(const std::string &filePath, const std::string &mapName) 
{
	GameMapStorage mapStorage;
	try {
		mapStorage.loadMap(filePath, m_map);
        m_currentMapName = mapName;
	}
	catch(std::invalid_argument &err) {
        std::cerr << err.what() << '\n';
	}
	catch(std::runtime_error &err) {
        std::cerr << err.what() << '\n';
	}
    
}

void GameMapMode::changeMap(const std::string &filePath, const std::string &mapName) 
{
    m_glPlayer->unloadGLPlayerObject();
    unloadGLMapObjects();
    loadMap(filePath, mapName);
    loadMapTextures();
    generateGLMapObjects();
    m_glPlayer->generateGLPlayerObject();
    m_glPlayer->setGLObjectPosition();
}

void GameMapMode::calculateGLTileCoord(const Point<> &tilePosition, GLfloat tileCoord[4][2]) 
{
    auto tileWidth = m_tileSize.tileWidth;
    auto tileHalfWidth = m_tileSize.tileHalfWidth;
    auto tileHalfHeight = m_tileSize.tileHalfHeight;
    float startPosX { -1.0F + tileHalfWidth };
    float startPosY { 1.0F - tileHalfHeight };
    float xConverted { static_cast<float>(tilePosition.x()) };
    float yConverted { static_cast<float>(tilePosition.y()) };

    tileCoord[0][0] = { -tileHalfWidth + startPosX + (tileWidth * xConverted) };                /* Top Left point */
    tileCoord[0][1] = { tileHalfHeight + startPosY - ((tileHalfHeight * 2) * yConverted) };     /* Top Left point */
    tileCoord[1][0] = { tileHalfWidth + startPosX + (tileWidth * xConverted) };                 /* Top Right point */
    tileCoord[1][1] = { tileHalfHeight + startPosY - ((tileHalfHeight * 2) * yConverted) };     /* Top Right point */
    tileCoord[2][0] = { tileHalfWidth + startPosX + (tileWidth * xConverted) };                 /* Bottom Right point */
    tileCoord[2][1] = { -tileHalfHeight + startPosY - ((tileHalfHeight * 2) * yConverted) };    /* Bottom Right point */
    tileCoord[3][0] = { -tileHalfWidth + startPosX + (tileWidth * xConverted) };                /* Bottom Left point */
    tileCoord[3][1] = { -tileHalfHeight + startPosY - ((tileHalfHeight * 2) * yConverted) };    /* Bottom Left point */
}

void GameMapMode::unloadGLMapObjects() 
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

void GameMapMode::loadMapTextures() 
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

void GameMapMode::generateGLMapObjects() 
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

void GameMapMode::onCharacterWindowClose()
{
    m_isCharacterWindowDisplayed = false;
    m_inputMode = GameMapInputMode::Map;

}

void GameMapMode::onInventoryWindowClose()
{
    m_isInventoryDisplayed = false;
    m_inputMode = GameMapInputMode::Map;
}

void GameMapMode::mainMenuPopupClicked(size_t choice)
{
    switch (choice)
    {
    case 0:
        toggleInventoryWindow();
        break;
    case 1:
        toggleCharacterWindow();
        break;
    case 2:
        mainMenuPopupCanceled();
        break;
    default:
        break;
    }
}

void GameMapMode::mainMenuPopupCanceled()
{
    m_inputMode = GameMapInputMode::Map;
}
