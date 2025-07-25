#pragma once

#include <GL/glew.h>
#include <memory>
#include <string>
#include <boost/signals2.hpp>
#include "glObjectService.hpp"
#include "glTextureService.hpp"
#include "player.hpp"
#include "point.hpp"
#include "size.hpp"
#include "texture.hpp"
#include "tileSize.hpp"

namespace thewarrior::ui {

enum class PlayerMovement { None, MoveLeft, MoveRight, MoveUp, MoveDown };
enum class PlayerFacing { Left, Up, Right, Down };

struct MovingResult {
    bool needToRefreshTexture;
};

class GLPlayer : public thewarrior::models::Player {
 public:
    explicit GLPlayer(const std::string &name);
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    const thewarrior::models::Texture &getTexture() const;
    thewarrior::models::Point<> getGridPosition() const;
    thewarrior::models::Point<float> getGLObjectPositionWithMovement() const;
    bool isInMovement() const;
    bool isRunning() const;
    bool isFacing(PlayerFacing direction);
    void initialize(const std::string &resourcesPath);
    void generateGLPlayerObject();
    void unloadGLPlayerObject();
    void setTexture(const thewarrior::models::TextureInfo &textureInfo);
    void applyCurrentGLTexture(const GLTextureService &textureService);
    void setGridPosition(thewarrior::models::Point<> position);
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
    void onGameWindowSizeChanged(const thewarrior::models::Size<> &);
    void onGameWindowTileSizeChanged(const TileSize &tileSize);
    void onGameWindowUpdate(float delta_time);
    boost::signals2::signal<void()> m_playerMoveCompleted;

 private:
    GLObject glObject;
    unsigned int glTextureId;
    thewarrior::models::Point<> m_coord;
    float m_xMove;
    float m_yMove;
    TileSize m_tileSize;
    PlayerMovement m_playerMovement;
    PlayerFacing m_playerFacing;
    bool m_isInClimbingMode;
    bool m_isInRunningMode;
    std::string m_textureName;
    int m_baseTextureIndex;
    int m_currentMovementTextureIndex;
    std::shared_ptr<thewarrior::models::Texture> m_texture;
    GLTextureService m_textureService;
};

}  // namespace thewarrior::ui
