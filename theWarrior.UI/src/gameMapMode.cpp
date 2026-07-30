#include <fmt/core.h>
#include <fmt/format.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "facingUtils.hpp"
#include "gameMapMode.hpp"
#include "gameMap.hpp"
#include "gameMapStorage.hpp"
#include "gameState.hpp"
#include "glNPC.hpp"
#include "glPlayer.hpp"
#include "goldFoundMessageDTO.hpp"
#include "itemFoundMessageDTO.hpp"
#include "openChestActionProperties.hpp"
#include "openChestActionPropertiesConverter.hpp"
#include "npcDialogueMessageDTO.hpp"
#include "mapTile.hpp"
#include "monsterZone.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include "randomUtils.hpp"

using namespace thewarrior::models;
using namespace thewarrior::ui::controllers;
using namespace thewarrior::ui::models;
using namespace thewarrior::storage;
using namespace thewarrior::utils;

namespace thewarrior::ui
{

    GameMapMode::GameMapMode()
    {
        m_choicePopup.m_choiceClicked.connect(boost::bind(&GameMapMode::choicePopupClicked, this, boost::placeholders::_1));
        m_choicePopup.m_cancelClicked.connect(boost::bind(&GameMapMode::choicePopupCanceled, this));
        m_glBattleWindow.m_battleCompleted.connect(boost::bind(&GameMapMode::onBattleCompleted, this));
    }

    GameMapMode::~GameMapMode()
    {
        m_glPlayer->unloadGLPlayerObject();
        unloadGLMapObjects();
        unloadGLNPCObjects();
        Mix_FreeMusic(m_mapMusic);
        Mix_FreeMusic(m_battleMusic);
        Mix_FreeMusic(m_sleepMusic);
    }

    bool GameMapMode::initialize(const std::string &resourcesPath,
                                 const GameState &gameState,
                                 std::shared_ptr<GLTextService> textService,
                                 std::shared_ptr<InputDevicesState> inputDevicesState)
    {
        auto worldState = std::make_shared<WorldState>(gameState.getWorldState());
        m_controller.initialize(resourcesPath, worldState);
        if (!loadStores())
        {
            return false;
        }
        if (!loadShaders())
        {
            return false;
        }
        m_textureService.setResourcesPath(resourcesPath);
        loadItemStoreTextures();
        loadMonsterStoreTextures();
        m_map = std::make_shared<GameMap>(1, 1);
        m_glPlayer = std::make_shared<GLPlayer>(gameState.getPlayer());
        m_glPlayer->initialize(resourcesPath, worldState);
        m_glPlayer->m_playerMoveCompleted.connect(boost::bind(&GameMapMode::onPlayerMoveCompleted, this));
        m_glFormService->initialize(m_shaderProgram, textService);
        m_glBattleWindow.initialize(resourcesPath, m_glPlayer, textService, m_controller.getMonsterStore(), &m_texturesGLMonsterStore, inputDevicesState);
        m_glCharacterWindow.initialize(resourcesPath, m_glPlayer, textService, m_controller.getItemStore(), &m_texturesGLItemStore, inputDevicesState);
        m_glInventory.initialize(resourcesPath, m_glPlayer, textService, m_controller.getItemStore(), &m_texturesGLItemStore, inputDevicesState);
        m_glInventory.setInventory(m_glPlayer->getInventory());
        m_textBox->initialize(m_controller.getResourcesPath(),
                              textService,
                              m_controller.getItemStore(),
                              &m_texturesGLItemStore);
        m_inputDevicesState = inputDevicesState;
        m_choicePopup.initialize(resourcesPath, textService, inputDevicesState);
        if (!m_screenOverlay.initialize(resourcesPath)) {
            std::cerr << m_screenOverlay.getLastError() << "\n";
            return false;
        }

        const auto mapName = worldState->getCurrentMapName();
        loadMap(fmt::format("{0}/maps/{1}", resourcesPath, mapName), mapName);

        // Battle music & sounds
        m_battleMusic = Mix_LoadMUS(fmt::format("{0}/sounds/battle.mp3", m_controller.getResourcesPath()).c_str());
        if (m_battleMusic == nullptr) {
            std::cerr << fmt::format("Mix_LoadMUS error: {0}\n", Mix_GetError());
            return false;
        }

        // Sleep music
        m_sleepMusic = Mix_LoadMUS(fmt::format("{0}/sounds/sleep.mp3", m_controller.getResourcesPath()).c_str());
        if (m_sleepMusic == nullptr) {
            std::cerr << fmt::format("Mix_LoadMUS error: {0}\n", Mix_GetError());
            return false;
        }

        generateGLMapObjects();
        m_glCharacterWindow.onCloseEvent.connect(boost::bind(&GameMapMode::onCharacterWindowClose, this));
        m_glInventory.onCloseEvent.connect(boost::bind(&GameMapMode::onInventoryWindowClose, this));
        return true;
    }

    bool GameMapMode::initShaders(const std::string &resourcesPath) {
        m_shaderProgram = std::make_shared<GLShaderProgram>(fmt::format("{0}/shaders/window_330_vs.glsl", resourcesPath),
                                                            fmt::format("{0}/shaders/window_330_fs.glsl", resourcesPath));
        if (!m_shaderProgram->compileShaders()) {
            m_lastError = m_shaderProgram->getLastError();
            return false;
        }
        if (!m_shaderProgram->linkShaders({"vertex"})) {
            m_lastError = m_shaderProgram->getLastError();
            return false;
        }
        m_glBattleWindow.initShader(m_shaderProgram);
        if (!m_glBattleWindow.initBattleShaders(resourcesPath)) {
            m_lastError = m_glBattleWindow.getLastError();
            return false;
        }
        m_choicePopup.initShader(m_shaderProgram);
        m_glCharacterWindow.initShader(m_shaderProgram);
        m_glInventory.initShader(m_shaderProgram);
        return true;
    }

    const std::string &GameMapMode::getLastError() const {
        return m_lastError;
    }

    void GameMapMode::processEvents(SDL_Event &e) {
        if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_i && !m_controller.isMessageDisplayed()) {
            toggleInventoryWindow();
        } else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_c && !m_controller.isMessageDisplayed()) {
            toggleCharacterWindow();
        } else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE) {
            if (m_controller.isMessageDisplayed()) {
                completeCurrentMessage(true);
            }
            else if (m_inputMode == GameMapInputMode::Map) {
                showMainMenu();
                m_inputDevicesState->reset();
            }
        }
    }

    void GameMapMode::update()
    {
        // Block input during sleep
        if (m_sleepSequenceState != SleepSequenceState::Inactive) {
            calculateTilesToDisplay();
            return;
        }
        switch (m_inputMode)
        {
        case GameMapInputMode::Map:
            if (m_controller.isMessageDisplayed())
            {
                if (m_inputDevicesState->getButtonAState() == InputElementState::Released)
                {
                    actionButtonPressed();
                }
                break;
            }
            if (m_inputDevicesState->isADirectionKeyPressed())
            {
                if (!m_glPlayer->isInMovement())
                {
                    if (m_inputDevicesState->getUpPressed())
                    {
                        moveUpPressed();
                    }
                    else if (m_inputDevicesState->getDownPressed())
                    {
                        moveDownPressed();
                    }
                    else if (m_inputDevicesState->getLeftPressed())
                    {
                        moveLeftPressed();
                    }
                    else if (m_inputDevicesState->getRightPressed())
                    {
                        moveRightPressed();
                    }
                }
            }
            if (m_inputDevicesState->getButtonAState() == InputElementState::Released)
            {
                actionButtonPressed();
            }
            if (m_inputDevicesState->isADirectionKeyPressed())
            {
                if (m_inputDevicesState->getButtonBState() == InputElementState::Pressed ||
                    m_inputDevicesState->getKeyShiftState() == InputElementState::Pressed)
                {
                    m_glPlayer->enableRunMode();
                }
                else
                {
                    m_glPlayer->disableRunMode();
                }
            }
            if (m_inputDevicesState->getButtonCState() == InputElementState::Released)
            {
                showMainMenu();
            }
            break;
        case GameMapInputMode::MainMenuPopup:
        case GameMapInputMode::ConversationChoice:
            m_choicePopup.update();
            break;
        case GameMapInputMode::InventoryWindow:
            m_glInventory.update();
            break;
        case GameMapInputMode::CharacterWindow:
            m_glCharacterWindow.update();
            break;
        case GameMapInputMode::Battle:
            m_glBattleWindow.update();
            break;
        }
        calculateTilesToDisplay();
    }

    void GameMapMode::gameWindowSizeChanged(const Size<> &size)
    {
        m_screenSize = size;
        calculateTileSize();
        m_glFormService->gameWindowSizeChanged(size);
        unloadGLMapObjects();
        generateGLMapObjects();
        m_glPlayer->onGameWindowTileSizeChanged(m_tileSize);
        for (auto &npc : m_glNPCs)
        {
            npc.onGameWindowTileSizeChanged(m_tileSize);
        }
        m_glInventory.gameWindowSizeChanged(size);
        m_glBattleWindow.gameWindowSizeChanged(size);
        m_glCharacterWindow.gameWindowSizeChanged(size);
        m_choicePopup.gameWindowSizeChanged(size);
        m_textBox->gameWindowSizeChanged(size);
    }

    void GameMapMode::onGameWindowUpdate(float delta_time)
    {
        m_glPlayer->onGameWindowUpdate(delta_time);
        m_textBox->update(delta_time);
        for (auto &npc : m_glNPCs)
        {
            npc.onGameWindowUpdate(delta_time,
                                   *m_map,
                                   m_controller.getWorldState()->getAllNPCPositions(),
                                   m_controller.getPlayerPosition(),
                                   m_textureService);
        }
        updateSleepSequence(delta_time);
    }

    void GameMapMode::calculateTileSize()
    {
        Size<float> screenSizeFloat(static_cast<float>(m_screenSize.width()), static_cast<float>(m_screenSize.height()));

        m_tileSize.tileWidth = (1.0F / (screenSizeFloat.width() / 51.2F)) * 2.0F;
        m_tileSize.tileHalfWidth = m_tileSize.tileWidth / 2.0F;
        m_tileSize.tileHalfHeight = (screenSizeFloat.width() * m_tileSize.tileHalfWidth) / screenSizeFloat.height();
    }

    void GameMapMode::showMainMenu()
    {
        m_inputMode = GameMapInputMode::MainMenuPopup;
        m_choicePopup.preparePopup({"Inventory", "Character", "Back", "Save", "Exit Game"}, "Menu");
        m_choicePopup.generateGLElements();
    }

    void GameMapMode::toggleInventoryWindow()
    {
        if (m_inputMode == GameMapInputMode::Map || m_inputMode == GameMapInputMode::MainMenuPopup)
        {
            m_inputMode = GameMapInputMode::InventoryWindow;
            m_glInventory.generateGLInventory();
        }
        else if (m_inputMode == GameMapInputMode::InventoryWindow)
        {
            m_inputMode = GameMapInputMode::Map;
            onInventoryWindowClose();
        }
    }

    void GameMapMode::toggleCharacterWindow()
    {
        if (m_inputMode == GameMapInputMode::Map || m_inputMode == GameMapInputMode::MainMenuPopup)
        {
            m_inputMode = GameMapInputMode::CharacterWindow;
            m_glCharacterWindow.generateGLElements();
        }
        else if (m_inputMode == GameMapInputMode::CharacterWindow)
        {
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
        for (auto &item : m_glTiles)
        {
            // Display only the tiles that are visible on the screen
            if (item.x < m_tileCoordToDisplay[0] || item.x > m_tileCoordToDisplay[1] || item.y < m_tileCoordToDisplay[2] || item.y > m_tileCoordToDisplay[3])
            {
                continue;
            }
            glBindVertexArray(item.glMainObject.vao);
            glBindBuffer(GL_ARRAY_BUFFER, item.glMainObject.vboPosition);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, item.glMainObject.vboColor);
            glEnableVertexAttribArray(1);
            if (item.tile.hasTexture())
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[item.tile.getTextureName()]);
                glBindBuffer(GL_ARRAY_BUFFER, item.glMainObject.vboTexture);
                glEnableVertexAttribArray(2);
            }
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glBindTexture(GL_TEXTURE_2D, 0);
            // Object
            if (item.tile.hasTexture() && item.tile.hasObjectTexture())
            {
                if (!item.tile.getObjectAbovePlayer())
                {
                    drawObjectTile(item);
                }
                else
                {
                    // Add the tile to a list that will be drawed after the player
                    tilesToBeDrawedAfterPlayer.push_back(&item);
                }
            }
        }
        // Render the player
        m_glPlayer->draw();
        for (const auto &npc : m_glNPCs)
        {
            npc.draw();
        }

        // Draw all the object that appears above the player
        for (auto *item : tilesToBeDrawedAfterPlayer)
        {
            drawObjectTile(*item);
        }
        // Display messages
        auto currentMessage = m_controller.getCurrentMessage();
        if (currentMessage) {
            if (!currentMessage->isDisplayed) {
                m_textBox->generateMessage(currentMessage);
                m_controller.displayCurrentMessage();
            }
            // Display the message
            m_textBox->draw();
            if (currentMessage->isExpired) {
                completeCurrentMessage(false);
            }
        }
        if (m_inputMode == GameMapInputMode::MainMenuPopup) {
            m_choicePopup.render();
        }
        if (m_inputMode == GameMapInputMode::ConversationChoice) {
            m_choicePopup.render();
        }
        if (m_inputMode == GameMapInputMode::InventoryWindow) {
            m_glInventory.render();
        }
        if (m_inputMode == GameMapInputMode::CharacterWindow) {
            m_glCharacterWindow.render();
        }
        if (m_inputMode == GameMapInputMode::Battle) {
            m_glBattleWindow.render();
        }
        if (m_sleepSequenceState != SleepSequenceState::Inactive) {
            m_screenOverlay.render(m_sleepOverlayOpacity);
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
        glBindVertexArray(tile.vaoSecondObject);
        glBindBuffer(GL_ARRAY_BUFFER, tile.glMainObject.vboPosition);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, tile.glMainObject.vboColor);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, tile.vboSecondTextureObject);
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
        if (m_controller.isMessageDisplayed())
        {
            completeCurrentMessage(true);
        }
        else
        {
            if (stopFacingNPCWandering())
            {
                return;
            }

            // Check if the tile in front of the player has an action-button trigger.
            Point<> tilePositionToProcess = getPlayerFacingTilePosition();
            if (tilePositionToProcess.x() >= 0 &&
                tilePositionToProcess.y() >= 0 &&
                static_cast<unsigned int>(tilePositionToProcess.x()) < m_map->getWidth() &&
                static_cast<unsigned int>(tilePositionToProcess.y()) < m_map->getHeight()) {
                auto &tile = m_map->getTileForEditing(tilePositionToProcess);
                auto actionButtonTrigger = tile.findConstTrigger(MapTileTriggerEvent::ActionButtonPressed);
                if (actionButtonTrigger.has_value())
                {
                    processMapTileTrigger(
                        actionButtonTrigger.value(),
                        &tile,
                        tilePositionToProcess);
                }
            }
        }
    }

    bool GameMapMode::stopFacingNPCWandering()
    {
        const auto isWithinMapBounds = [this](const Point<> &position)
        {
            return position.x() >= 0 &&
                   position.y() >= 0 &&
                   static_cast<unsigned int>(position.x()) < m_map->getWidth() &&
                   static_cast<unsigned int>(position.y()) < m_map->getHeight();
        };

        auto interactionPosition = getPlayerFacingTilePosition();
        if (!isWithinMapBounds(interactionPosition))
        {
            return false;
        }

        const auto worldState = m_controller.getWorldState();
        const auto findNPCAtPosition = [this, &worldState](const Point<> &position)
        {
            const Point<size_t> convertedPosition(static_cast<size_t>(position.x()),
                                                  static_cast<size_t>(position.y()));
            return std::ranges::find_if(m_glNPCs, [&worldState, convertedPosition](const GLNPC &npc)
                                        { return worldState->getNPCPosition(npc.getId()) == convertedPosition; });
        };

        auto iter = findNPCAtPosition(interactionPosition);
        if (iter == m_glNPCs.end())
        {
            const auto &facingTile = m_map->getTileFromCoord(interactionPosition);
            if (!facingTile.getAllowsInteractionThrough())
            {
                return false;
            }

            const auto playerPosition = m_controller.getPlayerPosition();
            interactionPosition.setX(interactionPosition.x() + interactionPosition.x() - playerPosition.x());
            interactionPosition.setY(interactionPosition.y() + interactionPosition.y() - playerPosition.y());
            if (!isWithinMapBounds(interactionPosition))
            {
                return false;
            }

            iter = findNPCAtPosition(interactionPosition);
        }

        if (iter == m_glNPCs.end())
        {
            return false;
        }

        iter->stopWandering();
        iter->face(getNPCFacingOppositeOfPlayer(m_glPlayer->getFacing()), m_textureService);

        // TODO: Remove this
        /*const auto &dialogueLines = iter->getDialogueLines();
        if (!dialogueLines.empty()) {
            auto msg = std::make_unique<NPCDialogueMessageDTO>();
            msg->npcId = iter->getId();
            msg->dialogueLines = dialogueLines;
            msg->message = dialogueLines.front();
            m_controller.addMessageToPipeline(std::move(msg));
        } else {
            restoreNPCDefaultBehavior(iter->getId());
        }*/
        const auto &scenarios = iter->getConversationScenarios();

        if (scenarios.empty())
        {
            restoreNPCDefaultBehavior(iter->getId());
            return true;
        }

        if (!m_conversationController.start(
                iter->getId(),
                scenarios.front()))
        {
            m_lastError = m_conversationController.getLastError();
            std::cerr << m_lastError << "\n";
            restoreNPCDefaultBehavior(iter->getId());
            return true;
        }

        processCurrentConversationNode();
        return true;
    }

    Point<> GameMapMode::getPlayerFacingTilePosition() const
    {
        Point<> tilePosition = m_controller.getPlayerPosition();
        if (m_glPlayer->isFacing(PlayerFacing::Up))
        {
            tilePosition.setY(tilePosition.y() - 1);
        }
        else if (m_glPlayer->isFacing(PlayerFacing::Down))
        {
            tilePosition.setY(tilePosition.y() + 1);
        }
        else if (m_glPlayer->isFacing(PlayerFacing::Left))
        {
            tilePosition.setX(tilePosition.x() - 1);
        }
        else if (m_glPlayer->isFacing(PlayerFacing::Right))
        {
            tilePosition.setX(tilePosition.x() + 1);
        }
        return tilePosition;
    }

    void GameMapMode::moveUpPressed()
    {
        // Check if there is an action
        const auto playerCoord = m_controller.getPlayerPosition();
        const auto tile = m_map->getTileFromCoord(playerCoord);
        auto moveUpTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveUpPressed);
        if (moveUpTrigger.has_value())
        {
            processMapTileTrigger(moveUpTrigger.value());
        }
        else
        {
            const auto targetCoord = Point<int>(playerCoord.x(), playerCoord.y() - 1);
            // Check if there an NPC on the tile we are planning to move
            const bool isTileOccupyByNPC = m_controller.isTileOccupyByNPC(targetCoord);
            if (m_map->canSteppedOnTile(targetCoord) && !isTileOccupyByNPC)
            {
                m_glPlayer->moveUp();
            }
        }
        m_glPlayer->faceUp();
        m_glPlayer->applyCurrentGLTexture(m_textureService);
    }

    void GameMapMode::moveDownPressed()
    {
        // Check if there is an action
        const auto playerCoord = m_controller.getPlayerPosition();
        const auto tile = m_map->getTileFromCoord(playerCoord);
        auto moveDownTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveDownPressed);
        if (moveDownTrigger.has_value())
        {
            processMapTileTrigger(moveDownTrigger.value());
        }
        else
        {
            const auto targetCoord = Point<int>(playerCoord.x(), playerCoord.y() + 1);
            // Check if there an NPC on the tile we are planning to move
            const bool isTileOccupyByNPC = m_controller.isTileOccupyByNPC(targetCoord);
            if (m_map->canSteppedOnTile(targetCoord) && !isTileOccupyByNPC)
            {
                m_glPlayer->moveDown(tile.getIsWallToClimb());
            }
        }
        if (tile.getIsWallToClimb())
        {
            m_glPlayer->faceUp();
        }
        else
        {
            m_glPlayer->faceDown();
        }
        m_glPlayer->applyCurrentGLTexture(m_textureService);
    }

    void GameMapMode::moveLeftPressed()
    {
        // Check if there is an action
        const auto playerCoord = m_controller.getPlayerPosition();
        const auto tile = m_map->getTileFromCoord(playerCoord);
        auto moveLeftTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveLeftPressed);
        if (moveLeftTrigger.has_value())
        {
            processMapTileTrigger(moveLeftTrigger.value());
        }
        else
        {
            const auto targetCoord = Point<int>(playerCoord.x() - 1, playerCoord.y());
            // Check if there an NPC on the tile we are planning to move
            const bool isTileOccupyByNPC = m_controller.isTileOccupyByNPC(targetCoord);
            if (m_map->canSteppedOnTile(targetCoord) && !isTileOccupyByNPC)
            {
                m_glPlayer->moveLeft();
            }
        }
        if (tile.getIsWallToClimb())
        {
            m_glPlayer->faceUp();
        }
        else
        {
            m_glPlayer->faceLeft();
        }
        m_glPlayer->applyCurrentGLTexture(m_textureService);
    }

    void GameMapMode::moveRightPressed()
    {
        // Check if there is an action
        const auto playerCoord = m_controller.getPlayerPosition();
        const auto tile = m_map->getTileFromCoord(playerCoord);
        auto moveRightTrigger = tile.findConstTrigger(MapTileTriggerEvent::MoveRightPressed);
        if (moveRightTrigger.has_value())
        {
            processMapTileTrigger(moveRightTrigger.value());
        }
        else
        {
            const auto targetCoord = Point<int>(playerCoord.x() + 1, playerCoord.y());
            // Check if there an NPC on the tile we are planning to move
            const bool isTileOccupyByNPC = m_controller.isTileOccupyByNPC(targetCoord);
            if (m_map->canSteppedOnTile(targetCoord) && !isTileOccupyByNPC)
            {
                m_glPlayer->moveRight();
            }
        }
        if (tile.getIsWallToClimb())
        {
            m_glPlayer->faceUp();
        }
        else
        {
            m_glPlayer->faceRight();
        }
        m_glPlayer->applyCurrentGLTexture(m_textureService);
    }

    void GameMapMode::processAction(MapTileTriggerAction action, std::map<std::string, std::string> properties, MapTile *tile, Point<> tilePosition)
    {
        switch (action)
        {
        case MapTileTriggerAction::ChangeMap:
            if (properties.at("playerFacing") == "0")
            {
                m_glPlayer->faceUp();
            }
            else if (properties.at("playerFacing") == "1")
            {
                m_glPlayer->faceDown();
            }
            else if (properties.at("playerFacing") == "2")
            {
                m_glPlayer->faceLeft();
            }
            else if (properties.at("playerFacing") == "3")
            {
                m_glPlayer->faceRight();
            }
            m_controller.setPlayerPosition(Point<>(stoi(properties.at("playerX")), stoi(properties.at("playerY"))));
            changeMap(fmt::format("{0}/maps/{1}", m_controller.getResourcesPath(), properties.at("mapFileName")), properties.at("mapFileName"));
            break;
        case MapTileTriggerAction::OpenChest:
        {
            // Convert the properties to an OpenChestActionProperties object
            auto props = OpenChestActionPropertiesConverter::fromMap(properties);
            // Check if the item has already been taken
            auto tileIndex = m_map->getTileIndexFromCoord(tilePosition);
            if (!m_controller.isTileActionAlreadyProcessed(m_controller.getCurrentMapName(), tileIndex))
            {
                if (props.getContentType() == ChestContentType::Item)
                {
                    const auto &itemIdInside = props.getItemId();
                    // Find the item in the item store
                    const auto item = m_controller.findItem(itemIdInside);
                    m_controller.addItemToInventory(dynamic_cast<Player *>(m_glPlayer.get()), itemIdInside);
                    // Display the item on the screen
                    auto msg = std::make_unique<ItemFoundMessageDTO>();
                    msg->message = fmt::format("You found a {0}!", item.name);
                    msg->maxDurationInMilliseconds = 2000;
                    msg->itemId = item.id;
                    msg->textureName = item.textureName;
                    m_controller.addMessageToPipeline(std::move(msg));
                }
                else if (props.getContentType() == ChestContentType::Gold)
                {
                    m_controller.awardGoldToPlayer(dynamic_cast<Player *>(m_glPlayer.get()), props.getGoldAmount());
                    // Display the gold message on the screen
                    auto msg = std::make_unique<GoldFoundMessageDTO>();
                    msg->goldAmount = props.getGoldAmount();
                    msg->message = fmt::format("You found {0} gold {1}!",
                                               msg->goldAmount,
                                               msg->goldAmount == 1 ? "piece" : "pieces");
                    msg->maxDurationInMilliseconds = 2000;
                    m_controller.addMessageToPipeline(std::move(msg));
                }
                m_controller.addTileActionProcessed(m_controller.getCurrentMapName(), tileIndex);
            }
            if (tile != nullptr)
            {
                tile->setObjectTextureIndex(stoi(properties.at("objectTextureIndexOpenedChest")));
                // Update the GLTile
                auto iter = find_if(m_glTiles.begin(), m_glTiles.end(), [&tilePosition](GLTile &glTile)
                                    { return glTile.x == tilePosition.x() && glTile.y == tilePosition.y(); });
                if (iter != m_glTiles.end())
                {
                    GLTile &glTileToUpdate = *iter;
                    glTileToUpdate.tile = *tile;
                    GLfloat tileCoord[4][2];
                    calculateGLTileCoord(Point<int>(glTileToUpdate.x, glTileToUpdate.y), tileCoord);
                    auto newChestTexture = m_map->getTextureByName(tile->getObjectTextureName());
                    GenerateGLObjectInfo infoGenObject{
                        &glTileToUpdate.glSecondObject,
                        newChestTexture.has_value() ? &newChestTexture.value().get() : nullptr,
                        tile->getObjectTextureIndex(),
                        &glTileToUpdate.vaoSecondObject,
                        &glTileToUpdate.vboSecondTextureObject};
                    GLObjectService::generateGLObject(infoGenObject, tileCoord, m_texColorBuf);
                }
            }
        }
        break;
        default:
            break;
        }
    }

    void GameMapMode::processMapTileTrigger(
        const MapTileTrigger &trigger,
        MapTile *tile,
        Point<> tilePosition) {
        if (const auto *conversationAction =
                boost::get<StartConversationMapTileTriggerAction>(
                    &trigger.getActionData())) {
            if (!m_conversationController.start(
                    "",
                    conversationAction->scenario)) {
                failConversation(m_conversationController.getLastError());
                return;
            }

            processCurrentConversationNode();
            return;
        }

        processAction(
            trigger.getAction(),
            trigger.getActionProperties(),
            tile,
            tilePosition);
    }

    void GameMapMode::checkForMonsterEncounter(const MapTile &tile)
    {
        auto zones = m_map->getMonsterZones();
        if (tile.getMonsterZoneIndex() == -1 ||
            tile.getMonsterZoneIndex() >= static_cast<int>(zones.size()))
        {
            return;
        }
        // Check if we encounter a monster
        const MonsterZone zone = m_map->getMonsterZones().at(static_cast<size_t>(tile.getMonsterZoneIndex()));
        std::uniform_int_distribution<> distributionEncounter(static_cast<int>(zone.getRatioEncounter()),
                                                              static_cast<int>(zone.getRatioEncounterOn()));
        bool encounterAMonster = distributionEncounter(RandomGenerator::instance()) == 1;
        if (!encounterAMonster)
        {
            return;
        }
        // Check if we get a Rare, lessThanNormal or Normal monster
        std::uniform_int_distribution<> distributionMonsterType(1, 50);
        int monsterTypeResult = distributionMonsterType(RandomGenerator::instance());
        MonsterEncounterRatio typeOfMonsterEncountered = [&monsterTypeResult]() -> MonsterEncounterRatio
        {
            if (monsterTypeResult >= 1 && monsterTypeResult <= 37)
            {
                return MonsterEncounterRatio::Normal;
            }
            else if (monsterTypeResult >= 38 && monsterTypeResult <= 49)
            {
                return MonsterEncounterRatio::LessThanNormal;
            }
            return MonsterEncounterRatio::Rare;
        }();
        // Get a list of available Monsters by type
        const auto monsterIdEncounter = selectMonsterEncounter(zone.getMonsterEncounters(), typeOfMonsterEncountered);
        m_inputMode = GameMapInputMode::Battle;
        m_glBattleWindow.prepareWindow(monsterIdEncounter);
        Mix_FadeInMusic(m_battleMusic, -1, 2000);

        // std::cout << "MonsterZone: " << tile.getMonsterZoneIndex() <<
        //" Name: " << zone.getName() <<
        //" Encounter: " << (encounterAMonster ? "Yes" : "No") <<
        //" TypeOfEncounter: " << typeOfMonsterEncountered <<
        //" MonsterId: " << monsterIdEncounter <<
        // std::endl;
    }

    std::string GameMapMode::selectMonsterEncounter(const std::vector<MonsterZoneMonsterEncounter> &encounters,
                                                    MonsterEncounterRatio ratio)
    {
        std::vector<MonsterZoneMonsterEncounter> rareMonsters;
        std::vector<MonsterZoneMonsterEncounter> lessThanNormalMonsters;
        std::vector<MonsterZoneMonsterEncounter> normalMonsters;
        std::copy_if(encounters.begin(),
                     encounters.end(),
                     std::back_inserter(rareMonsters),
                     [](const MonsterZoneMonsterEncounter &encounter)
                     {
                         return encounter.getEncounterRatio() == MonsterEncounterRatio::Rare;
                     });
        std::copy_if(encounters.begin(),
                     encounters.end(),
                     std::back_inserter(lessThanNormalMonsters),
                     [](const MonsterZoneMonsterEncounter &encounter)
                     {
                         return encounter.getEncounterRatio() == MonsterEncounterRatio::LessThanNormal;
                     });
        std::copy_if(encounters.begin(),
                     encounters.end(),
                     std::back_inserter(normalMonsters),
                     [](const MonsterZoneMonsterEncounter &encounter)
                     {
                         return encounter.getEncounterRatio() == MonsterEncounterRatio::Normal;
                     });
        const auto &monsterListToUse = [rareMonsters, lessThanNormalMonsters, normalMonsters, ratio]()
        {
            if (ratio == MonsterEncounterRatio::Rare && !rareMonsters.empty())
            {
                return rareMonsters;
            }
            if ((ratio == MonsterEncounterRatio::Rare || ratio == MonsterEncounterRatio::LessThanNormal) && !lessThanNormalMonsters.empty())
            {
                return lessThanNormalMonsters;
            }
            return normalMonsters;
        }();
        if (monsterListToUse.size() == 1)
        {
            return monsterListToUse.at(0).getMonsterId();
        }
        std::uniform_int_distribution<> distributionMonsterSelection(1, static_cast<int>(monsterListToUse.size()));
        auto monsterRandomIndex = static_cast<size_t>(distributionMonsterSelection(RandomGenerator::instance()));
        return monsterListToUse.at(monsterRandomIndex - 1).getMonsterId();
    }

    void GameMapMode::loadMap(const std::string &filePath, const std::string &mapName)
    {
        GameMapStorage mapStorage;
        try
        {
            mapStorage.loadMap(filePath, m_map);
            m_controller.setCurrentMapName(mapName);
            m_controller.clearNPCsWorldState();
            loadMapTextures();

            m_glNPCs.clear();
            for (const auto &npc : m_map->getNPCs())
            {
                // Find the related texture
                const auto &textureResult = m_map->getTextureByName(npc.getTextureName());
                if (!textureResult.has_value())
                {
                    throw std::runtime_error(fmt::format("Unable to find the texture: {0}", npc.getTextureName()));
                }
                GLNPC glNPC(npc, textureResult->get());
                glNPC.setGLTextureId(m_texturesGLMap[npc.getTextureName()]);
                glNPC.initialize(m_tileSize, m_controller.getWorldState());
                m_glNPCs.push_back(glNPC);
            }
            const std::string mapMusicFilename = m_map->getMusicFilename();
            if (m_mapMusic != nullptr)
            {
                Mix_FreeMusic(m_mapMusic);
                m_mapMusic = nullptr;
            }
            if (!mapMusicFilename.empty())
            {
                m_mapMusic = Mix_LoadMUS(fmt::format("{0}/sounds/{1}", m_controller.getResourcesPath(), mapMusicFilename).c_str());
                if (m_mapMusic == nullptr)
                {
                    std::cerr << fmt::format("Mix_LoadMUS error: {0}\n", Mix_GetError());
                }
                else
                {
                    Mix_FadeInMusic(m_mapMusic, -1, 2000);
                }
            }
        }
        catch (std::invalid_argument &err)
        {
            std::cerr << err.what() << '\n';
        }
        catch (std::runtime_error &err)
        {
            std::cerr << err.what() << '\n';
        }
    }

    void GameMapMode::changeMap(const std::string &filePath, const std::string &mapName)
    {
        m_glPlayer->unloadGLPlayerObject();
        unloadGLMapObjects();
        unloadGLNPCObjects();
        loadMap(filePath, mapName);
        unloadGLMapObjects();
        generateGLMapObjects();
        m_glPlayer->generateGLPlayerObject();
        m_glPlayer->setGLObjectPosition();
    }

    void GameMapMode::calculateGLTileCoord(const Point<> &tilePosition, GLfloat tileCoord[4][2])
    {
        auto tileWidth = m_tileSize.tileWidth;
        auto tileHalfWidth = m_tileSize.tileHalfWidth;
        auto tileHalfHeight = m_tileSize.tileHalfHeight;
        float startPosX{-1.0F + tileHalfWidth};
        float startPosY{1.0F - tileHalfHeight};
        float xConverted{static_cast<float>(tilePosition.x())};
        float yConverted{static_cast<float>(tilePosition.y())};

        tileCoord[0][0] = {-tileHalfWidth + startPosX + (tileWidth * xConverted)};             /* Top Left point */
        tileCoord[0][1] = {tileHalfHeight + startPosY - ((tileHalfHeight * 2) * yConverted)};  /* Top Left point */
        tileCoord[1][0] = {tileHalfWidth + startPosX + (tileWidth * xConverted)};              /* Top Right point */
        tileCoord[1][1] = {tileHalfHeight + startPosY - ((tileHalfHeight * 2) * yConverted)};  /* Top Right point */
        tileCoord[2][0] = {tileHalfWidth + startPosX + (tileWidth * xConverted)};              /* Bottom Right point */
        tileCoord[2][1] = {-tileHalfHeight + startPosY - ((tileHalfHeight * 2) * yConverted)}; /* Bottom Right point */
        tileCoord[3][0] = {-tileHalfWidth + startPosX + (tileWidth * xConverted)};             /* Bottom Left point */
        tileCoord[3][1] = {-tileHalfHeight + startPosY - ((tileHalfHeight * 2) * yConverted)}; /* Bottom Left point */
    }

    void GameMapMode::calculateTilesToDisplay()
    {
        float screenWidth = static_cast<float>(m_screenSize.width());
        float screenHeight = static_cast<float>(m_screenSize.height());
        float screenCenterX = screenWidth / 2.0F;
        float screenCenterY = screenHeight / 2.0F;
        float tileWidthInPx = screenWidth * (m_tileSize.tileHalfWidth * 2.0F) / 2.0F;
        float tileHeightInPx = screenHeight * (m_tileSize.tileHalfHeight * 2.0F) / 2.0F;
        auto playerPosition = m_glPlayer->getGLObjectPositionWithMovement();
        m_tileCoordToDisplay.at(0) = static_cast<int>(floor(playerPosition.x() - (screenCenterX / tileWidthInPx)));
        m_tileCoordToDisplay.at(1) = static_cast<int>(ceil(playerPosition.x() - 1 + (screenCenterX / tileWidthInPx)));
        // If you reach the left of the map, add the tiles to display to the right
        if (m_tileCoordToDisplay.at(0) < 0)
        {
            m_tileCoordToDisplay.at(1) += m_tileCoordToDisplay.at(0) * -1;
        }
        // If you reach the right of the map, add the tiles to display to the left
        if (m_tileCoordToDisplay.at(1) > static_cast<int>(m_map->getWidth()) - 1)
        {
            m_tileCoordToDisplay.at(0) -= m_tileCoordToDisplay.at(1) - static_cast<int>(m_map->getWidth());
            m_tileCoordToDisplay.at(1) = static_cast<int>(m_map->getWidth()) - 1;
        }
        // Set map boundary if necessary
        if (m_tileCoordToDisplay.at(0) < 0)
        {
            m_tileCoordToDisplay.at(0) = 0;
        }
        m_tileCoordToDisplay.at(2) = static_cast<int>(floor(playerPosition.y() - (screenCenterY / tileHeightInPx)));
        m_tileCoordToDisplay.at(3) = static_cast<int>(ceil(playerPosition.y() - 1 + (screenCenterY / tileHeightInPx)));
        // If you reach the top of the map, add the tiles to display to the bottom
        if (m_tileCoordToDisplay.at(2) < 0)
        {
            m_tileCoordToDisplay.at(3) += m_tileCoordToDisplay.at(2) * -1;
        }
        // If you reach the bottom of the map, add the tiles to display to the top
        if (m_tileCoordToDisplay.at(3) > static_cast<int>(m_map->getHeight()) - 1)
        {
            m_tileCoordToDisplay.at(2) -= m_tileCoordToDisplay.at(3) - static_cast<int>(m_map->getHeight());
            m_tileCoordToDisplay.at(3) = static_cast<int>(m_map->getHeight()) - 1;
        }
        // Set map boundary if necessary
        if (m_tileCoordToDisplay.at(2) < 0)
        {
            m_tileCoordToDisplay.at(2) = 0;
        }
    }

    void GameMapMode::unloadGLMapObjects()
    {
        for (auto &item : m_glTiles)
        {
            if (item.glMainObject.vboPosition)
                glDeleteBuffers(1, &item.glMainObject.vboPosition);
            if (item.glMainObject.vboColor)
                glDeleteBuffers(1, &item.glMainObject.vboColor);
            if (item.glMainObject.vboTexture)
                glDeleteBuffers(1, &item.glMainObject.vboTexture);
            if (item.glMainObject.vao)
                glDeleteVertexArrays(1, &item.glMainObject.vao);
            if (item.glSecondObject.vboPosition)
                glDeleteBuffers(1, &item.glSecondObject.vboPosition);
            if (item.glSecondObject.vboColor)
                glDeleteBuffers(1, &item.glSecondObject.vboColor);
            if (item.glSecondObject.vboTexture)
                glDeleteBuffers(1, &item.glSecondObject.vboTexture);
            if (item.glSecondObject.vao)
                glDeleteVertexArrays(1, &item.glSecondObject.vao);
            if (item.vboSecondTextureObject)
                glDeleteBuffers(1, &item.vboSecondTextureObject);
            if (item.tile.hasObjectTexture())
            {
                if (item.vaoSecondObject)
                    glDeleteVertexArrays(1, &item.vaoSecondObject);
            }
        }
        m_glTiles.clear();
    }

    void GameMapMode::unloadGLNPCObjects()
    {
        for (auto &glNPC : m_glNPCs)
        {
            glNPC.unloadGLObject();
        }
    }

    void GameMapMode::loadMapTextures()
    {
        // Clear existing textures in graphics memory
        for (auto &glTexture : m_texturesGLMap)
        {
            m_textureService.unloadTexture(glTexture.second);
        }
        m_texturesGLMap.clear();
        // Load texture in graphics memory
        for (const auto &texture : m_map->getTextures())
        {
            const auto &textureName{texture.getName()};
            m_textureService.loadTexture(texture, m_texturesGLMap[textureName]);
        }
    }

    bool GameMapMode::loadStores()
    {
        if (!m_controller.loadItemStore(fmt::format("{0}/items/itemstore.itm", m_controller.getResourcesPath())))
        {
            std::cerr << "Unable to load the item store : " << m_controller.getLastError() << "\n";
            return false;
        }
        if (!m_controller.loadMonsterStore(fmt::format("{0}/monsters/monsterstore.mon", m_controller.getResourcesPath())))
        {
            std::cerr << "Unable to load the monster store : " << m_controller.getLastError() << "\n";
            return false;
        }
        return true;
    }

    bool GameMapMode::loadShaders()
    {
        if (!m_tileService->initShader(fmt::format("{0}/shaders/tile_330_vs.glsl", m_controller.getResourcesPath()),
                                       fmt::format("{0}/shaders/tile_330_fs.glsl", m_controller.getResourcesPath())))
        {
            std::cerr << m_tileService->getLastError() << "\n";
            return false;
        }
        if (!m_textBox->initShader(fmt::format("{0}/shaders/textbox_330_vs.glsl", m_controller.getResourcesPath()),
                                   fmt::format("{0}/shaders/textbox_330_fs.glsl", m_controller.getResourcesPath())))
        {
            std::cerr << m_textBox->getLastError() << "\n";
            return false;
        }
        return true;
    }

    void GameMapMode::loadItemStoreTextures()
    {
        // Clear existing textures in graphics memory
        for (auto &glTexture : m_texturesGLItemStore)
        {
            glDeleteTextures(1, &glTexture.second);
        }
        m_texturesGLItemStore.clear();
        for (const auto &texture : m_controller.getItemStore()->getTextureContainer().getTextures())
        {
            const auto &textureName{texture.getName()};
            m_textureService.loadTexture(texture, m_texturesGLItemStore[textureName]);
        }
    }

    void GameMapMode::loadMonsterStoreTextures()
    {
        // Clear existing textures in graphics memory
        for (auto &glTexture : m_texturesGLMonsterStore)
        {
            glDeleteTextures(1, &glTexture.second);
        }
        m_texturesGLMonsterStore.clear();
        for (const auto &texture : m_controller.getMonsterStore()->getTextureContainer().getTextures())
        {
            const auto &textureName{texture.getName()};
            m_textureService.loadTexture(texture, m_texturesGLMonsterStore[textureName]);
        }
    }

    void GameMapMode::generateGLMapObjects()
    {
        int indexRow{0};
        for (const auto &row : m_map->getTiles())
        {
            int indexCol{0};
            for (const auto &tile : row)
            {
                GLTile glTile;
                glTile.x = indexCol;
                glTile.y = indexRow;
                glTile.tile = tile;
                GLfloat tileCoord[4][2];
                calculateGLTileCoord(Point<int>(indexCol, indexRow), tileCoord);
                auto tileTexture = m_map->getTextureByName(tile.getTextureName());
                GenerateGLObjectInfo infoGenTexture{
                    &glTile.glMainObject,
                    tileTexture.has_value() ? &tileTexture.value().get() : nullptr,
                    tile.getTextureIndex()};
                GLObjectService::generateGLObject(infoGenTexture, tileCoord, m_texColorBuf);

                if (glTile.tile.hasObjectTexture())
                {
                    auto objectTexture = m_map->getTextureByName(tile.getObjectTextureName());
                    GenerateGLObjectInfo infoGenObject{
                        &glTile.glSecondObject,
                        objectTexture.has_value() ? &objectTexture.value().get() : nullptr,
                        tile.getObjectTextureIndex(),
                        &glTile.vaoSecondObject,
                        &glTile.vboSecondTextureObject};
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
        switch (choice) {
        case 0:
            toggleInventoryWindow();
            break;
        case 1:
            toggleCharacterWindow();
            break;
        case 2:
            mainMenuPopupCanceled();
            break;
        case 3:
            m_inputMode = GameMapInputMode::Map;
            if (!m_controller.saveGameState(*m_glPlayer)) {
                auto msg = std::make_unique<MessageDTO>();
                msg->message = fmt::format("An error occurred while saving the game:\n{0}", m_controller.getLastError());
                msg->maxDurationInMilliseconds = 20000;
                m_controller.addMessageToPipeline(std::move(msg));
            } else {
                auto msg = std::make_unique<MessageDTO>();
                msg->message = "Game saved successfully!";
                msg->maxDurationInMilliseconds = 3000;
                m_controller.addMessageToPipeline(std::move(msg));
            }
            break;
        case 4:
            exitGameAndReturnToMainMenu();
            break;
        default:
            break;
        }
    }

    void GameMapMode::mainMenuPopupCanceled()
    {
        m_inputMode = GameMapInputMode::Map;
    }

    void GameMapMode::exitGameAndReturnToMainMenu() {
        quitRequested();
    }

    void GameMapMode::onPlayerMoveCompleted() {
        const auto &tile = m_map->getTileFromCoord(m_controller.getPlayerPosition());
        auto steppedOnTrigger = tile.findConstTrigger(MapTileTriggerEvent::SteppedOn);
        if (steppedOnTrigger.has_value()) {
            processMapTileTrigger(steppedOnTrigger.value());
        } else {
            checkForMonsterEncounter(tile);
        }
    }

    void GameMapMode::onBattleCompleted() {
        m_inputMode = GameMapInputMode::Map;
        Mix_FadeOutMusic(1000);
        Mix_FadeInMusic(m_mapMusic, -1, 2000);
    }

    void GameMapMode::completeCurrentMessage(bool allowTextReveal) {
        if (allowTextReveal && m_textBox->isRevealingText()) {
            m_textBox->revealAllText();
            return;
        }

        auto currentMessage = m_controller.getCurrentMessage();
        if (currentMessage == nullptr) {
            return;
        }

        // Non-conversation messages keep their existing behavior.
        if (currentMessage->getType() != MessageDTOType::NPCDialogueMessage) {
            m_controller.acknowledgeMessage();
            m_textBox->clearMessage();
            return;
        }

        // Advance within the same dialogue node.
        if (m_controller.currentNPCDialogueMessageHasNextPage()) {
            m_controller.advanceCurrentNPCDialogueMessagePage();
            return;
        }

        // Copy this before acknowledging the message.
        std::string npcId;
        if (const auto *npcMessage = dynamic_cast<const NPCDialogueMessageDTO *>(currentMessage.get())) {
            npcId = npcMessage->npcId;
        }

        const bool hasActiveConversation = m_conversationController.getState() == ConversationState::Running;

        // The final page has been consumed.
        m_controller.acknowledgeMessage();
        m_textBox->clearMessage();

        if (hasActiveConversation) {
            // Complete only the dialogue node, not the whole conversation.
            if (!m_conversationController.completeCurrentNode()) {
                // Copy the error before failConversation() stops and clears
                // the conversation controller.
                const auto error = m_conversationController.getLastError();

                failConversation(error);
                return;
            }

            // This will display the next dialogue/choice, execute an action,
            // or call finishConversation() if the scenario is complete.
            processCurrentConversationNode();
            return;
        }

        // Compatibility path for NPC dialogue not managed by a scenario.
        if (!npcId.empty()) {
            restoreNPCDefaultBehavior(npcId);
        }
    }

    void GameMapMode::choicePopupClicked(std::size_t choiceIndex) {
        if (m_inputMode == GameMapInputMode::ConversationChoice) {
            conversationChoiceSelected(choiceIndex);
            return;
        }

        if (m_inputMode == GameMapInputMode::MainMenuPopup) {
            mainMenuPopupClicked(choiceIndex);
        }
    }

    void GameMapMode::choicePopupCanceled() {
        if (m_inputMode == GameMapInputMode::ConversationChoice) {
            // Keep the choice displayed.
            return;
        }

        if (m_inputMode == GameMapInputMode::MainMenuPopup) {
            mainMenuPopupCanceled();
        }
    }

    void GameMapMode::restoreNPCDefaultBehavior(const std::string &npcId) {
        constexpr float DialogueCompletedPauseInSeconds = 3.0F;
        auto iter = std::ranges::find_if(m_glNPCs, [&npcId](const GLNPC &npc)
                                         { return npc.getId() == npcId; });

        if (iter == m_glNPCs.end()) {
            return;
        }

        iter->restoreDefaultStateAfterDelay(DialogueCompletedPauseInSeconds);
    }

    void GameMapMode::processCurrentConversationNode() {
        constexpr std::size_t MaxImmediateNodes = 100;

        for (std::size_t count = 0; count < MaxImmediateNodes; ++count) {
            if (m_conversationController.getState() == ConversationState::Completed) {
                finishConversation();
                return;
            }

            if (m_conversationController.getState() == ConversationState::Error) {
                failConversation(m_conversationController.getLastError());
                return;
            }

            const auto *node = m_conversationController.getCurrentNode();
            if (node == nullptr) {
                finishConversation();
                return;
            }

            const auto &content = node->getContent();

            if (const auto *dialogue = boost::get<ConversationDialogue>(&content)) {
                displayConversationDialogue(*dialogue);
                return; // Wait for the player.
            }

            if (const auto *choice = boost::get<thewarrior::models::ConversationChoice>(&content)) {
                displayConversationChoice(*choice);
                return; // Wait for the player.
            }

            if (const auto *action = boost::get<ConversationAction>(&content)) {
                if (!executeConversationAction(*action)) {
                    return;
                }

                if (m_sleepSequenceState != SleepSequenceState::Inactive) {
                    return; // Wait for the sleep sequence.
                }

                // The controller now points to the next node.
                continue;
            }
        }

        m_lastError = "The conversation exceeded the immediate-node execution limit.";
        failConversation(m_lastError);
    }

    void GameMapMode::finishConversation()
    {
        const auto npcId = m_conversationController.getNPCId();

        // Remove any conversation UI still displayed.
        if (m_controller.isMessageDisplayed()) {
            m_controller.acknowledgeMessage();
            m_textBox->clearMessage();
        }

        // Return input handling from a choice popup to the map.
        m_inputMode = GameMapInputMode::Map;

        // Clear the conversation after retrieving its NPC ID.
        m_conversationController.stop();

        // Allow the NPC to resume its configured behavior.
        if (!npcId.empty()) {
            restoreNPCDefaultBehavior(npcId);
        }
    }

    void GameMapMode::failConversation(const std::string &error) {
        finishConversation();
        std::cerr << error << "\n";
    }

    void GameMapMode::displayConversationDialogue(const ConversationDialogue &dialogue) {

        if (dialogue.lines.empty()) {
            // An empty dialogue has nothing to display, so treat it as completed.
            if (!m_conversationController.completeCurrentNode()) {
                m_lastError = m_conversationController.getLastError();
                finishConversation();
                return;
            }

            processCurrentConversationNode();
            return;
        }

        auto message = std::make_unique<NPCDialogueMessageDTO>();
        message->npcId = m_conversationController.getNPCId();
        message->dialogueLines = dialogue.lines;
        message->message = dialogue.lines.front();

        m_controller.addMessageToPipeline(std::move(message));
    }

    void GameMapMode::displayConversationChoice(const thewarrior::models::ConversationChoice &choice)
    {

        if (choice.options.empty()) {
            failConversation("The conversation choice contains no options.");
            return;
        }

        std::vector<std::string> optionTexts;
        optionTexts.reserve(choice.options.size());

        for (const auto &option : choice.options) {
            optionTexts.push_back(option.text);
        }

        m_choicePopup.preparePopup(
            std::move(optionTexts),
            "",
            choice.prompt
        );

        m_choicePopup.generateGLElements();
        m_inputMode = GameMapInputMode::ConversationChoice;
    }

    void GameMapMode::conversationChoiceSelected(std::size_t choiceIndex)
    {

        if (m_conversationController.getState() != ConversationState::Running) {
            m_lastError = "A conversation choice was selected without an active conversation.";
            std::cerr << m_lastError << "\n";
            m_inputMode = GameMapInputMode::Map;
            return;
        }

        if (!m_conversationController.selectChoice(choiceIndex)) {
            const auto error = m_conversationController.getLastError();

            failConversation(error);
            return;
        }

        // Stop displaying and updating the choice popup.
        m_inputMode = GameMapInputMode::Map;

        processCurrentConversationNode();
    }

    bool GameMapMode::executeConversationAction(const ConversationAction &action)
    {
        /*if (const auto *rewardAction =
                boost::get<RewardAction>(&action)) {
            return executeRewardAction(*rewardAction);
        }

        if (const auto *storyAction =
                boost::get<ProgressStoryLineAction>(&action)) {
            return executeProgressStoryLineAction(*storyAction);
        }*/

        if (const auto *restAction =
                boost::get<RestRequestedAction>(&action)) {
            return executeRestRequestedAction(*restAction);
        }

        failConversation("Unknown conversation action type.");
        return false;
    }

    bool GameMapMode::completeConversationAction() {
        if (m_conversationController.completeCurrentNode()) {
            return true;
        }

        const auto error = m_conversationController.getLastError();

        failConversation(error);
        return false;
    }

    bool GameMapMode::followConversationActionFailure(const ConversationNodeTransition &transition) {

        if (m_conversationController.followTransition(transition)) {
            return true;
        }

        const auto error = m_conversationController.getLastError();

        failConversation(error);
        return false;
    }

    bool GameMapMode::executeRestRequestedAction(const RestRequestedAction &action) {
        auto *player = dynamic_cast<Player *>(m_glPlayer.get());

        if (player == nullptr) {
            failConversation("Unable to execute rest action: player is unavailable.");
            return false;
        }

        const auto playerGold = static_cast<unsigned int>(player->getGold());
        if (playerGold < action.goldCost) {
            return followConversationActionFailure(action.failureTransition);
        }

        player->removeGold(static_cast<int>(action.goldCost));

        if (action.restoreHealth) {
            const auto stats = player->getStats();
            player->restoreHealth(stats.maxHealth - stats.health);
        }

        m_restoreHealthAfterSleep = action.restoreHealth;
        m_sleepSequenceState = SleepSequenceState::FadingOut;
        m_sleepOverlayOpacity = 0.0F;

        Mix_FadeOutMusic(500);

        return true;
    }

    void GameMapMode::updateSleepSequence(float deltaTime)
    {
        constexpr float FadeDurationSeconds = 0.75F;
        constexpr float FadeSpeed = 1.0F / FadeDurationSeconds;

        switch (m_sleepSequenceState) {
        case SleepSequenceState::Inactive:
            return;

        case SleepSequenceState::FadingOut:
            m_sleepOverlayOpacity += FadeSpeed * deltaTime;

            if (m_sleepOverlayOpacity >= 1.0F) {
                m_sleepOverlayOpacity = 1.0F;

                restorePlayerAfterSleep();

                if (m_sleepMusic != nullptr) {
                    Mix_PlayMusic(m_sleepMusic, 0);
                    m_sleepSequenceState = SleepSequenceState::PlayingJingle;
                } else {
                    m_sleepSequenceState = SleepSequenceState::FadingIn;
                }
            }
            return;

        case SleepSequenceState::PlayingJingle:
            if (Mix_PlayingMusic() == 0) {
                m_sleepSequenceState = SleepSequenceState::FadingIn;

                if (m_mapMusic != nullptr) {
                    Mix_FadeInMusic(m_mapMusic, -1, 1000);
                }
            }
            return;

        case SleepSequenceState::FadingIn:
            m_sleepOverlayOpacity -= FadeSpeed * deltaTime;

            if (m_sleepOverlayOpacity <= 0.0F) {
                m_sleepOverlayOpacity = 0.0F;
                m_sleepSequenceState =
                    SleepSequenceState::Inactive;

                completeSleepAction();
            }
            return;
        }
    }

    void GameMapMode::restorePlayerAfterSleep()
    {
        if (!m_restoreHealthAfterSleep) {
            return;
        }

        const auto stats = m_glPlayer->getStats();
        m_glPlayer->restoreHealth(stats.maxHealth - stats.health);
    }

    void GameMapMode::completeSleepAction() {
        m_restoreHealthAfterSleep = false;

        if (!completeConversationAction()) {
            return;
        }

        processCurrentConversationNode();
    }

} // namespace thewarrior::ui
