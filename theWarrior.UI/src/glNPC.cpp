#include <algorithm>
#include <deque>
#include <memory>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "glNPC.hpp"
#include "direction.hpp"
#include "gameMap.hpp"
#include "glTextureService.hpp"
#include "movementResult.hpp"
#include "npc.hpp"
#include "point.hpp"
#include "randomUtils.hpp"
#include "texture.hpp"

using thewarrior::models::Direction;
using thewarrior::models::GameMap;
using thewarrior::models::NPC;
using thewarrior::models::NPCBehavior;
using thewarrior::models::NPCFacing;
using thewarrior::models::Point;
using thewarrior::models::Texture;
using thewarrior::utils::RandomGenerator;

namespace thewarrior::ui
{

    size_t getPointKey(const Point<size_t> &point);
    Point<> toIntPoint(const Point<size_t> &point);
    std::vector<Point<size_t>> getNeighbors(const Point<size_t> &point);

    GLNPC::GLNPC(const NPC &npc, const thewarrior::models::Texture &texture)
        : NPC(npc), m_texture(texture), m_direction(Direction::None) {}

    void GLNPC::initialize(const TileSize &tileSize,
                           std::shared_ptr<thewarrior::models::WorldState> worldState)
    {
        m_tileSize = tileSize;
        m_worldState = std::move(worldState);
        m_direction = Direction::None;
        m_xMove = 0.0F;
        m_yMove = 0.0F;
        m_currentPath.clear();
        m_idleTimeRemaining = getRandomIdleTime();
        m_worldState->setNPCPosition(getId(), getSpawnPosition());
        setFacing(getDefaultFacing());
        m_currentMovementTextureIndex = getCurrentFacingTextureIndex();
        generateGLObject();
        setGLObjectPosition();
    }

    void GLNPC::draw() const
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

    void GLNPC::generateGLObject()
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
            {-tileHalfWidth + startPosX,
             tileHalfHeight + startPosY}, /* Top Left point */
            {tileHalfWidth + startPosX,
             tileHalfHeight + startPosY}, /* Top Right point */
            {tileHalfWidth + startPosX,
             -tileHalfHeight + startPosY}, /* Bottom Right point */
            {-tileHalfWidth + startPosX,
             -tileHalfHeight + startPosY}}; /* Bottom Left point */

        GenerateGLObjectInfo infoGenTexture{.glObject = &glObject,
                                            .texture = &m_texture,
                                            .textureIndex = m_currentMovementTextureIndex};
        GLObjectService::generateGLObject(infoGenTexture, tileCoord, texColorBuf);
    }

    void GLNPC::unloadGLObject()
    {
        glDeleteBuffers(1, &glObject.vboPosition);
        glDeleteBuffers(1, &glObject.vboColor);
        glDeleteBuffers(1, &glObject.vboTexture);
        glDeleteVertexArrays(1, &glObject.vao);
    }

    void GLNPC::setGLTextureId(unsigned int textureId) { glTextureId = textureId; }

    void GLNPC::setGLObjectPosition() const
    {
        glBindVertexArray(glObject.vao);

        auto tileWidth = m_tileSize.tileWidth;
        auto tileHeight = m_tileSize.tileHalfHeight * 2.0F;
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

        auto npcPosition = getPosition();
        for (int i = 0; i < 4; i++) {
            m_tileCoordBuf[i][0] += ((static_cast<float>(npcPosition.x()) - 1.0F) * tileWidth) +
                                    (m_xMove * tileWidth);
            m_tileCoordBuf[i][1] -= ((static_cast<float>(npcPosition.y()) * tileHeight) - tileHalfHeight) +
                                    (m_yMove * tileHeight);
        }
        glBindBuffer(GL_ARRAY_BUFFER, glObject.vboPosition);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), m_tileCoordBuf,
                     GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
    }

    void GLNPC::onGameWindowTileSizeChanged(const TileSize &tileSize)
    {
        m_tileSize = tileSize;
        unloadGLObject();
        generateGLObject();
        setGLObjectPosition();
    }

    bool GLNPC::isInMovement() const
    {
        return m_direction != Direction::None;
    }

    void GLNPC::stopWandering()
    {
        setCurrentBehavior(NPCBehavior::Stationary);
        m_currentPath.clear();
        m_idleTimeRemaining = 0.0F;
    }

    void GLNPC::restoreDefaultStateAfterDelay(float durationInSeconds) {
        m_hasPendingDefaultRestore = true;
        m_defaultRestoreTimeRemaining = durationInSeconds;
    }

    void GLNPC::face(thewarrior::models::NPCFacing facing, const GLTextureService &textureService)
    {
        setFacing(facing);
        applyCurrentGLTexture(textureService);
    }

    void GLNPC::onGameWindowUpdate(float deltaTime,
                                   const GameMap &map,
                                   const std::vector<Point<size_t>> &occupiedPositions,
                                   const Point<> &playerPosition,
                                   const GLTextureService &textureService) {
        if (isInMovement()) {
            const auto result = processMoving(deltaTime);
            if (result.positionChanged) {
                applyCurrentGLTexture(textureService);
            }
            setGLObjectPosition();
            return;
        }

        if (m_hasPendingDefaultRestore) {
            m_defaultRestoreTimeRemaining -= deltaTime;
            if (m_defaultRestoreTimeRemaining <= 0.0F) {
                if (getDefaultBehavior() == NPCBehavior::Stationary) {
                    face(getDefaultFacing(), textureService);
                }
                setCurrentBehavior(getDefaultBehavior());
                m_hasPendingDefaultRestore = false;
            }
            return;
        }

        if (getCurrentBehavior() != NPCBehavior::Wander || getWanderZone().empty()) {
            return;
        }

        if (!m_currentPath.empty()) {
            m_idleTimeRemaining -= deltaTime;
            if (m_idleTimeRemaining > 0.0F) {
                return;
            }

            const auto nextPosition = m_currentPath.front();
            if (isPositionOccupied(nextPosition, occupiedPositions, playerPosition)) {
                m_currentPath.clear();
                m_idleTimeRemaining = getRandomIdleTime();
                return;
            }
            moveToNextPathTile();
            applyCurrentGLTexture(textureService);
            setGLObjectPosition();
            return;
        }

        m_idleTimeRemaining -= deltaTime;
        if (m_idleTimeRemaining <= 0.0F) {
            if (!tryStartNextWanderPath(map)) {
                m_idleTimeRemaining = getRandomIdleTime();
                return;
            }
            if (!m_currentPath.empty()) {
                const auto nextPosition = m_currentPath.front();
                if (!isPositionOccupied(nextPosition, occupiedPositions, playerPosition)) {
                    moveToNextPathTile();
                    applyCurrentGLTexture(textureService);
                    setGLObjectPosition();
                }
            }
        }
    }

    const thewarrior::models::Point<size_t> &GLNPC::getPosition() const
    {
        return m_worldState->getNPCPosition(getId());
    }

    thewarrior::models::NPCFacing GLNPC::getFacing()
    {
        return getCurrentFacing();
    }

    void GLNPC::setFacing(thewarrior::models::NPCFacing facing)
    {
        setCurrentFacing(facing);
        m_currentMovementTextureIndex = getCurrentFacingTextureIndex();
        m_worldState->setNPCFacing(getId(), facing);
    }

    size_t getPointKey(const Point<size_t> &point)
    {
        return (point.y() << 32U) ^ point.x();
    }

    Point<> toIntPoint(const Point<size_t> &point)
    {
        return {static_cast<int>(point.x()), static_cast<int>(point.y())};
    }

    std::vector<Point<size_t>> getNeighbors(const Point<size_t> &point)
    {
        std::vector<Point<size_t>> neighbors;
        if (point.x() > 0U) {
            neighbors.emplace_back(point.x() - 1U, point.y());
        }
        if (point.y() > 0U) {
            neighbors.emplace_back(point.x(), point.y() - 1U);
        }
        neighbors.emplace_back(point.x() + 1U, point.y());
        neighbors.emplace_back(point.x(), point.y() + 1U);
        return neighbors;
    }

    bool GLNPC::tryStartNextWanderPath(const GameMap &map)
    {
        const auto start = getPosition();

        // Pick targets in random order so unreachable ones can be skipped.
        auto targetCandidates = getWanderZone();
        std::shuffle(targetCandidates.begin(), targetCandidates.end(), RandomGenerator::instance());

        // Keep only walkable tiles from the configured wandering zone.
        std::unordered_set<size_t> zoneKeys;
        for (const auto &point : getWanderZone()) {
            if (map.canSteppedOnTile(toIntPoint(point))) {
                zoneKeys.insert(getPointKey(point));
            }
        }
        if (!zoneKeys.contains(getPointKey(start))) {
            return false;
        }

        for (const auto &target : targetCandidates) {
            if (target == start || !zoneKeys.contains(getPointKey(target))) {
                continue;
            }

            // Breadth-first search for a path that stays inside the walkable wandering zone.
            std::deque<Point<size_t>> toVisit;
            std::unordered_set<size_t> visited;
            std::unordered_map<size_t, Point<size_t>> previous;
            toVisit.push_back(start);
            visited.insert(getPointKey(start));

            bool found = false;
            while (!toVisit.empty() && !found) {
                const auto current = toVisit.front();
                toVisit.pop_front();

                for (const auto &neighbor : getNeighbors(current)) {
                    const auto neighborKey = getPointKey(neighbor);
                    if (!zoneKeys.contains(neighborKey) || visited.contains(neighborKey)) {
                        continue;
                    }

                    visited.insert(neighborKey);
                    previous.insert_or_assign(neighborKey, current);
                    if (neighbor == target) {
                        found = true;
                        break;
                    }
                    toVisit.push_back(neighbor);
                }
            }

            if (!found) {
                continue;
            }

            // Rebuild the path from target back to start, then reverse it for movement.
            std::vector<Point<size_t>> path;
            auto current = target;
            while (current != start) {
                path.push_back(current);
                current = previous.at(getPointKey(current));
            }
            std::reverse(path.begin(), path.end());
            m_currentPath = path;
            return true;
        }
        return false;
    }

    void GLNPC::moveToNextPathTile()
    {
        if (m_currentPath.empty()) {
            return;
        }

        const auto currentPosition = getPosition();
        const auto nextPosition = m_currentPath.front();
        m_currentPath.erase(m_currentPath.begin());

        if (nextPosition.x() < currentPosition.x()) {
            m_direction = Direction::MoveLeft;
            m_xMove = 1.0F;
            setFacing(NPCFacing::Left);
            m_currentMovementTextureIndex = getBaseTextureIndex() + 24;
        } else if (nextPosition.x() > currentPosition.x()) {
            m_direction = Direction::MoveRight;
            m_xMove = -1.0F;
            setFacing(NPCFacing::Right);
            m_currentMovementTextureIndex = getBaseTextureIndex() + 12;
        } else if (nextPosition.y() < currentPosition.y()) {
            m_direction = Direction::MoveUp;
            m_yMove = 1.0F;
            setFacing(NPCFacing::Up);
            m_currentMovementTextureIndex = getBaseTextureIndex();
        } else if (nextPosition.y() > currentPosition.y()) {
            m_direction = Direction::MoveDown;
            m_yMove = -1.0F;
            setFacing(NPCFacing::Down);
            m_currentMovementTextureIndex = getBaseTextureIndex() + 36;
        }

        m_worldState->setNPCPosition(getId(), nextPosition);
    }

    MovementResult GLNPC::processMoving(float deltaTime)
    {
        MovementResult result{false};
        constexpr float Speed = 3.0F;

        if (m_direction == Direction::MoveUp) {
            m_yMove -= Speed * deltaTime;
            if (m_yMove < 0.0F) {
                m_yMove = 0.0F;
                m_direction = Direction::None;
            } else if (m_yMove < 0.3F) {
                if (m_currentMovementTextureIndex != getBaseTextureIndex() + 1) {
                    m_currentMovementTextureIndex = getBaseTextureIndex() + 1;
                    result.positionChanged = true;
                }
            } else if (m_yMove < 0.6F) {
                if (m_currentMovementTextureIndex != getBaseTextureIndex() + 2) {
                    m_currentMovementTextureIndex = getBaseTextureIndex() + 2;
                    result.positionChanged = true;
                }
            }
        } else if (m_direction == Direction::MoveDown) {
            m_yMove += Speed * deltaTime;
            if (m_yMove > 0.0F) {
                m_yMove = 0.0F;
                m_direction = Direction::None;
            } else if (m_yMove > -0.3F) {
                if (m_currentMovementTextureIndex != getBaseTextureIndex() + 37) {
                    m_currentMovementTextureIndex = getBaseTextureIndex() + 37;
                    result.positionChanged = true;
                }
            } else if (m_yMove > -0.6F) {
                if (m_currentMovementTextureIndex != getBaseTextureIndex() + 38) {
                    m_currentMovementTextureIndex = getBaseTextureIndex() + 38;
                    result.positionChanged = true;
                }
            }
        } else if (m_direction == Direction::MoveLeft) {
            m_xMove -= Speed * deltaTime;
            if (m_xMove < 0.0F) {
                m_xMove = 0.0F;
                m_direction = Direction::None;
            } else if (m_xMove < 0.3F) {
                if (m_currentMovementTextureIndex != getBaseTextureIndex() + 25) {
                    m_currentMovementTextureIndex = getBaseTextureIndex() + 25;
                    result.positionChanged = true;
                }
            } else if (m_xMove < 0.6F) {
                if (m_currentMovementTextureIndex != getBaseTextureIndex() + 26) {
                    m_currentMovementTextureIndex = getBaseTextureIndex() + 26;
                    result.positionChanged = true;
                }
            }
        } else if (m_direction == Direction::MoveRight) {
            m_xMove += Speed * deltaTime;
            if (m_xMove > 0.0F) {
                m_xMove = 0.0F;
                m_direction = Direction::None;
            } else if (m_xMove > -0.3F) {
                if (m_currentMovementTextureIndex != getBaseTextureIndex() + 13) {
                    m_currentMovementTextureIndex = getBaseTextureIndex() + 13;
                    result.positionChanged = true;
                }
            } else if (m_xMove > -0.6F) {
                if (m_currentMovementTextureIndex != getBaseTextureIndex() + 14) {
                    m_currentMovementTextureIndex = getBaseTextureIndex() + 14;
                    result.positionChanged = true;
                }
            }
        }

        if (m_direction == Direction::None) {
            m_idleTimeRemaining = getRandomIdleTime();
        }
        return result;
    }

    void GLNPC::applyCurrentGLTexture(const GLTextureService &textureService) const
    {
        GLfloat texCoordBuf[4][2];
        textureService.setTextureUVFromIndex(&m_texture, texCoordBuf, m_currentMovementTextureIndex);

        glBindVertexArray(glObject.vao);
        glBindBuffer(GL_ARRAY_BUFFER, glObject.vboTexture);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), texCoordBuf, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);
        glDisableVertexAttribArray(2);
    }

    float GLNPC::getRandomIdleTime() const
    {
        std::uniform_real_distribution<float> distribution(1.0F, 3.0F);
        return distribution(RandomGenerator::instance());
    }

    bool GLNPC::isPositionOccupied(const Point<size_t> &position,
                                   const std::vector<Point<size_t>> &occupiedPositions,
                                   const Point<> &playerPosition) const
    {
        if (playerPosition.x() >= 0 && playerPosition.y() >= 0 &&
            position == Point<size_t>(static_cast<size_t>(playerPosition.x()),
                                      static_cast<size_t>(playerPosition.y()))) {
            return true;
        }

        return std::ranges::any_of(occupiedPositions, [this, position](const Point<size_t> &occupiedPosition)
                                   { return occupiedPosition == position && occupiedPosition != getPosition(); });
    }

}  // namespace thewarrior::ui
