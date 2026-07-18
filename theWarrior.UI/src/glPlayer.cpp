#include <memory>
#include <string>
#include <utility>
#include "direction.hpp"
#include "glPlayer.hpp"
#include "movementResult.hpp"
#include "player.hpp"
#include "worldState.hpp"

using namespace std;
using namespace thewarrior::models;

namespace thewarrior::ui
{

    GLPlayer::GLPlayer(const std::string &name)
        : Player(name),
          m_xMove(0.0F),
          m_yMove(0.0F),
          m_tileSize({.tileWidth = 1.0F, .tileHalfWidth = 1.0F, .tileHalfHeight = 1.0F}),
          m_isInRunningMode(false),
          m_texture(nullptr) {}

    GLPlayer::GLPlayer(const thewarrior::models::Player &player)
        : Player(player),
          m_xMove(0.0F),
          m_yMove(0.0F),
          m_tileSize({.tileWidth = 1.0F, .tileHalfWidth = 1.0F, .tileHalfHeight = 1.0F}),
          m_isInRunningMode(false),
          m_texture(nullptr) {}

    const string &GLPlayer::getTextureName() const
    {
        return m_textureName;
    }

    int GLPlayer::getTextureIndex() const
    {
        return m_currentMovementTextureIndex;
    }

    const Texture &GLPlayer::getTexture() const
    {
        return *m_texture;
    }

    Point<float> GLPlayer::getGLObjectPositionWithMovement() const
    {
        Point<> playerPosition = m_worldState->getPlayerPosition();
        return {static_cast<float>(playerPosition.x()) + m_xMove,
                static_cast<float>(playerPosition.y()) + m_yMove};
    }

    bool GLPlayer::isInMovement() const
    {
        return m_direction != Direction::None;
    }

    bool GLPlayer::isRunning() const
    {
        return m_isInRunningMode;
    }

    void GLPlayer::initialize(const std::string &resourcePath,
                              std::shared_ptr<WorldState> worldState)
    {
        m_worldState = std::move(worldState);
        m_textureName = "NPC1";
        m_xMove = 0.0F;
        m_yMove = 0.0F;
        m_baseTextureIndex = 9;
        m_currentMovementTextureIndex = m_baseTextureIndex + 1;
        m_direction = Direction::None;
        m_textureService.setResourcesPath(resourcePath);

        setTexture({"playerTexture", "tileNPC1.png", 384, 1280, 32, 32});
        m_textureService.loadTexture(getTexture(), glTextureId);
        generateGLPlayerObject();
        setGLObjectPosition();
    }

    void GLPlayer::generateGLPlayerObject()
    {
        GLfloat texColorBuf[4][3]{{1.0F, 1.0F, 1.0F}, /* Red */
                                  {1.0F, 1.0F, 1.0F}, /* Green */
                                  {1.0F, 1.0F, 1.0F}, /* Blue */
                                  {1.0F, 1.0F, 1.0F}};
        auto tileHalfWidth = m_tileSize.tileHalfWidth;
        auto tileHalfHeight = m_tileSize.tileHalfHeight;

        float startPosX{-1.0F + tileHalfWidth};
        float startPosY{1.0F - tileHalfHeight};

        GLfloat tileCoord[4][2] = {
            {-tileHalfWidth + startPosX, tileHalfHeight + startPosY},   /* Top Left point */
            {tileHalfWidth + startPosX, tileHalfHeight + startPosY},    /* Top Right point */
            {tileHalfWidth + startPosX, -tileHalfHeight + startPosY},   /* Bottom Right point */
            {-tileHalfWidth + startPosX, -tileHalfHeight + startPosY}}; /* Bottom Left point */

        GenerateGLObjectInfo infoGenTexture {
            &glObject,
            m_texture.get(),
            m_currentMovementTextureIndex};
        GLObjectService::generateGLObject(infoGenTexture, tileCoord, texColorBuf);
    }

    void GLPlayer::unloadGLPlayerObject()
    {
        glDeleteBuffers(1, &glObject.vboPosition);
        glDeleteBuffers(1, &glObject.vboColor);
        glDeleteBuffers(1, &glObject.vboTexture);
        glDeleteVertexArrays(1, &glObject.vao);
    }

    void GLPlayer::setTexture(const TextureInfo &textureInfo)
    {
        m_texture = make_shared<Texture>(textureInfo);
    }

    void GLPlayer::applyCurrentGLTexture(const GLTextureService &textureService)
    {
        GLfloat texCoordBuf[4][2];
        if (m_texture && m_currentMovementTextureIndex != -1) {
            textureService.setTextureUVFromIndex(m_texture.get(), texCoordBuf, m_currentMovementTextureIndex);
        }
        glBindVertexArray(glObject.vao);

        glBindBuffer(GL_ARRAY_BUFFER, glObject.vboTexture);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), texCoordBuf, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);
        glDisableVertexAttribArray(2);
    }

    void GLPlayer::setGLObjectPosition()
    {
        glBindVertexArray(glObject.vao);

        auto tileWidth = m_tileSize.tileWidth;
        auto tileHalfWidth = m_tileSize.tileHalfWidth;
        auto tileHalfHeight = m_tileSize.tileHalfHeight;
        // Set Tile Coord To Origin
        GLfloat m_tileCoordBuf[4][2];
        float startPosX{-1.0F + tileHalfWidth};
        float startPosY{1.0F - tileHalfHeight};

        m_tileCoordBuf[0][0] = -tileHalfWidth + startPosX + tileWidth;
        m_tileCoordBuf[0][1] = tileHalfHeight + startPosY - tileHalfHeight;
        m_tileCoordBuf[1][0] = tileHalfWidth + startPosX + tileWidth;
        m_tileCoordBuf[1][1] = tileHalfHeight + startPosY - tileHalfHeight;
        m_tileCoordBuf[2][0] = tileHalfWidth + startPosX + tileWidth;
        m_tileCoordBuf[2][1] = -tileHalfHeight + startPosY - tileHalfHeight;
        m_tileCoordBuf[3][0] = -tileHalfWidth + startPosX + tileWidth;
        m_tileCoordBuf[3][1] = -tileHalfHeight + startPosY - tileHalfHeight;

        auto playerPosition = m_worldState->getPlayerPosition();
        for (int i = 0; i < 4; i++) {
            m_tileCoordBuf[i][0] += ((static_cast<float>(playerPosition.x()) - 1.0F) * tileWidth) +
                                    (m_xMove * tileWidth);
            m_tileCoordBuf[i][1] -= (((static_cast<float>(playerPosition.y()) * (tileHalfHeight * 2.0F)) - tileHalfHeight)) +
                                    (m_yMove * (tileHalfHeight * 2.0F));
        }
        glBindBuffer(GL_ARRAY_BUFFER, glObject.vboPosition);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), m_tileCoordBuf, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
    }

    void GLPlayer::draw() const
    {
        glBindTexture(GL_TEXTURE_2D, glTextureId);
        glBindVertexArray(glObject.vao);
        glBindBuffer(GL_ARRAY_BUFFER, glObject.vboPosition);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, glObject.vboColor);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, glObject.vboTexture);
        glEnableVertexAttribArray(2);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }

    void GLPlayer::moveUp()
    {
        m_direction = Direction::MoveUp;
        m_worldState->movePlayerUp();
        m_yMove = 1.0F;
        m_currentMovementTextureIndex = m_baseTextureIndex;
    }

    void GLPlayer::moveDown(bool isInClimbingMode)
    {
        m_direction = Direction::MoveDown;
        setClimbing(isInClimbingMode);
        m_worldState->movePlayerDown();
        m_yMove = -1.0F;
        if (isInClimbingMode) {
            // Face up
            m_currentMovementTextureIndex = m_baseTextureIndex;
        } else {
            m_currentMovementTextureIndex = m_baseTextureIndex + 36;
        }
    }

    void GLPlayer::moveLeft()
    {
        m_direction = Direction::MoveLeft;
        m_worldState->movePlayerLeft();
        m_xMove = 1.0F;
        m_currentMovementTextureIndex = m_baseTextureIndex + 24;
    }

    void GLPlayer::moveRight()
    {
        m_direction = Direction::MoveRight;
        m_worldState->movePlayerRight();
        m_xMove = -1.0F;
        m_currentMovementTextureIndex = m_baseTextureIndex + 12;
    }
    void GLPlayer::faceUp()
    {
        m_currentMovementTextureIndex = m_baseTextureIndex + 1;
        setFacing(PlayerFacing::Up);
    }

    void GLPlayer::faceDown()
    {
        m_currentMovementTextureIndex = m_baseTextureIndex + 37;
        setFacing(PlayerFacing::Down);
    }

    void GLPlayer::faceLeft()
    {
        m_currentMovementTextureIndex = m_baseTextureIndex + 25;
        setFacing(PlayerFacing::Left);
    }

    void GLPlayer::faceRight()
    {
        m_currentMovementTextureIndex = m_baseTextureIndex + 13;
        setFacing(PlayerFacing::Right);
    }

    void GLPlayer::enableRunMode()
    {
        m_isInRunningMode = true;
    }

    void GLPlayer::disableRunMode()
    {
        m_isInRunningMode = false;
    }

    MovementResult GLPlayer::processMoving(float delta_time)
    {
        MovementResult result{false};
        const float SPEED = m_isInRunningMode ? 11.0F : 7.0F;
        if (m_direction == Direction::MoveUp) {
            m_yMove -= SPEED * delta_time;
            if (m_yMove < 0.0F) {
                m_yMove = 0.0F;
                m_direction = Direction::None;
                setClimbing(false);
                m_playerMoveCompleted();
            } else if (m_yMove < 0.3F) {
                if (m_currentMovementTextureIndex != 1) {
                    m_currentMovementTextureIndex = m_baseTextureIndex + 1;
                    result.positionChanged = true;
                }
            } else if (m_yMove < 0.6F) {
                if (m_currentMovementTextureIndex != 2) {
                    m_currentMovementTextureIndex = m_baseTextureIndex + 2;
                    result.positionChanged = true;
                }
            }
        } else if (m_direction == Direction::MoveDown) {
            m_yMove += SPEED * delta_time;
            if (m_yMove > 0.0F) {
                m_yMove = 0.0F;
                m_direction = Direction::None;
                setClimbing(false);
                m_playerMoveCompleted();
            } else if (m_yMove > -0.3F) {
                if (isClimbing()) {
                    if (m_currentMovementTextureIndex != 1) {
                        m_currentMovementTextureIndex = m_baseTextureIndex + 1;
                        result.positionChanged = true;
                    }
                } else {
                    if (m_currentMovementTextureIndex != m_baseTextureIndex + 37) {
                        m_currentMovementTextureIndex = m_baseTextureIndex + 37;
                        result.positionChanged = true;
                    }
                }
            } else if (m_yMove > -0.6F) {
                if (isClimbing()) {
                    if (m_currentMovementTextureIndex != 2) {
                        m_currentMovementTextureIndex = m_baseTextureIndex + 2;
                        result.positionChanged = true;
                    }
                }
                else {
                    if (m_currentMovementTextureIndex != m_baseTextureIndex + 38) {
                        m_currentMovementTextureIndex = m_baseTextureIndex + 38;
                        result.positionChanged = true;
                    }
                }
            }
        } else if (m_direction == Direction::MoveLeft) {
            m_xMove -= SPEED * delta_time;
            if (m_xMove < 0.0F) {
                m_xMove = 0.0F;
                m_direction = Direction::None;
                setClimbing(false);
                m_playerMoveCompleted();
            } else if (m_xMove < 0.3F) {
                if (m_currentMovementTextureIndex != m_baseTextureIndex + 25) {
                    m_currentMovementTextureIndex = m_baseTextureIndex + 25;
                    result.positionChanged = true;
                }
            } else if (m_xMove < 0.6f) {
                if (m_currentMovementTextureIndex != m_baseTextureIndex + 26) {
                    m_currentMovementTextureIndex = m_baseTextureIndex + 26;
                    result.positionChanged = true;
                }
            }
        } else if (m_direction == Direction::MoveRight) {
            m_xMove += SPEED * delta_time;
            if (m_xMove > 0.0F) {
                m_xMove = 0.0F;
                m_direction = Direction::None;
                setClimbing(false);
                m_playerMoveCompleted();
            } else if (m_xMove > -0.3F) {
                if (m_currentMovementTextureIndex != m_baseTextureIndex + 13) {
                    m_currentMovementTextureIndex = m_baseTextureIndex + 13;
                    result.positionChanged = true;
                }
            } else if (m_xMove > -0.6F) {
                if (m_currentMovementTextureIndex != m_baseTextureIndex + 14) {
                    m_currentMovementTextureIndex = m_baseTextureIndex + 14;
                    result.positionChanged = true;
                }
            }
        }
        return result;
    }

    void GLPlayer::onGameWindowTileSizeChanged(const TileSize &tileSize)
    {
        m_tileSize = tileSize;
        unloadGLPlayerObject();
        generateGLPlayerObject();
        setGLObjectPosition();
    }

    void GLPlayer::onGameWindowUpdate(float delta_time)
    {
        if (isInMovement()) {
            MovementResult result{processMoving(delta_time)};
            if (result.positionChanged) {
                applyCurrentGLTexture(m_textureService);
            }
            setGLObjectPosition();
        }
    }

} // namespace thewarrior::ui
