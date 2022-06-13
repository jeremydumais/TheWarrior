#pragma once

#include "glObjectService.hpp"
#include "glTextureService.hpp"
#include "player.hpp"
#include "point.hpp"
#include "size.hpp"
#include "texture.hpp"
#include "tileSize.hpp"
#include <GL/glew.h>
#include "memory"
#include <string>

enum class PlayerMovement { None, MoveLeft, MoveRight, MoveUp, MoveDown };
enum class PlayerFacing { Left, Up, Right, Down };

struct MovingResult {
    bool needToRefreshTexture;
};

class GLPlayer : public Player 
{
public:
    explicit GLPlayer(const std::string &name);
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    const Texture &getTexture() const;
    Point<> getGridPosition() const;
    Point<float> getGLObjectPositionWithMovement() const;
    bool isInMovement() const;
    bool isRunning() const;
    bool isFacing(PlayerFacing direction);
    void initialize(const std::string &resourcesPath);
    void generateGLPlayerObject();
    void unloadGLPlayerObject();
    void setTexture(const TextureInfo &textureInfo);
    void applyCurrentGLTexture(const GLTextureService &textureService);
    void setGridPosition(Point<> position);
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
    void onGameWindowSizeChanged(const Size<> &);
    void onGameWindowTileSizeChanged(const TileSize &tileSize);
    void onGameWindowUpdate(float delta_time);
private:
    GLObject glObject;
    unsigned int glTextureId;
    Point<> m_coord;
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
    std::shared_ptr<Texture> m_texture;
    GLTextureService m_textureService;    
};


