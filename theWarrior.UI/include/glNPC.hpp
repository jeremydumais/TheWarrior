#pragma  once

#include <memory>
#include <vector>
#include "direction.hpp"
#include "gameMap.hpp"
#include "glObjectService.hpp"
#include "glTextureService.hpp"
#include "movementResult.hpp"
#include "npc.hpp"
#include "point.hpp"
#include "texture.hpp"
#include "tileSize.hpp"
#include "worldState.hpp"

namespace thewarrior::ui {

class GLNPC : public thewarrior::models::NPC {
 public:
     explicit GLNPC(const thewarrior::models::NPC &npc,
                    const thewarrior::models::Texture &texture);
     ~GLNPC() override = default;
    void initialize(const TileSize &tileSize,
                    std::shared_ptr<thewarrior::models::WorldState> m_worldState);
    void draw() const;
    void generateGLObject();
    void unloadGLObject();
    void setGLTextureId(unsigned int textureId);
    void setGLObjectPosition() const;
    void onGameWindowTileSizeChanged(const TileSize &tileSize);
    bool isInMovement() const;
    void stopWandering();
    void onGameWindowUpdate(float deltaTime,
                            const thewarrior::models::GameMap &map,
                            const std::vector<thewarrior::models::Point<size_t>> &occupiedPositions,
                            const thewarrior::models::Point<> &playerPosition,
                            const GLTextureService &textureService);

 private:
    std::shared_ptr<thewarrior::models::WorldState> m_worldState = nullptr;
    const thewarrior::models::Texture &m_texture;
    GLObject glObject;
    unsigned int glTextureId;
    TileSize m_tileSize;
    thewarrior::models::Direction m_direction;
    float m_xMove = 0.0F;
    float m_yMove = 0.0F;
    float m_idleTimeRemaining = 0.0F;
    int m_currentMovementTextureIndex = -1;
    std::vector<thewarrior::models::Point<size_t>> m_currentPath;
    const thewarrior::models::Point<size_t> &getPosition() const;
    thewarrior::models::NPCFacing getFacing();
    void setFacing(thewarrior::models::NPCFacing facing);
    bool tryStartNextWanderPath(const thewarrior::models::GameMap &map);
    void moveToNextPathTile();
    MovementResult processMoving(float deltaTime);
    void applyCurrentGLTexture(const GLTextureService &textureService) const;
    float getRandomIdleTime() const;
    bool isPositionOccupied(const thewarrior::models::Point<size_t> &position,
                            const std::vector<thewarrior::models::Point<size_t>> &occupiedPositions,
                            const thewarrior::models::Point<> &playerPosition) const;
};

}  // namespace thewarrior::ui
