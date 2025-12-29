#pragma once

#include <GL/glew.h>
#include <memory>
#include <string>
#include <boost/signals2.hpp>
#include "glObjectService.hpp"
#include "glTextureService.hpp"
#include "player.hpp"
#include "point.hpp"
#include "texture.hpp"
#include "tileSize.hpp"
#include "worldState.hpp"

namespace thewarrior::ui {

enum class PlayerMovement { None, MoveLeft, MoveRight, MoveUp, MoveDown };

struct MovingResult {
    bool needToRefreshTexture;
};

class GLPlayer : public thewarrior::models::Player {
 public:
    explicit GLPlayer(const std::string &name);
    explicit GLPlayer(const thewarrior::models::Player &player);
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    const thewarrior::models::Texture &getTexture() const;
    thewarrior::models::Point<float> getGLObjectPositionWithMovement() const;
    bool isInMovement() const;
    bool isRunning() const;
    void initialize(const std::string &resourcesPath,
                    std::shared_ptr<thewarrior::models::WorldState> worldState);
    void generateGLPlayerObject();
    void unloadGLPlayerObject();
    void setTexture(const thewarrior::models::TextureInfo &textureInfo);
    void applyCurrentGLTexture(const GLTextureService &textureService);
    void setGLObjectPosition();
    void draw();
    void moveUp();
    void moveDown(bool isInClimbingMode);
    void moveLeft();
    void moveRight();
    void faceUp();
    void faceDown();
    void faceLeft();
    void faceRight();
    void enableRunMode();
    void disableRunMode();
    MovingResult processMoving(float delta_time);
    void onGameWindowTileSizeChanged(const TileSize &tileSize);
    void onGameWindowUpdate(float delta_time);
    boost::signals2::signal<void()> m_playerMoveCompleted;

 private:
    std::shared_ptr<thewarrior::models::WorldState> m_worldState = nullptr;
    GLObject glObject;
    unsigned int glTextureId;
    float m_xMove;
    float m_yMove;
    TileSize m_tileSize;
    PlayerMovement m_playerMovement;
    bool m_isInRunningMode;
    std::string m_textureName;
    int m_baseTextureIndex;
    int m_currentMovementTextureIndex;
    std::shared_ptr<thewarrior::models::Texture> m_texture;
    GLTextureService m_textureService;
};

}  // namespace thewarrior::ui
