#pragma once

#include "glObjectService.hpp"
#include "glTextureService.hpp"
#include "point.hpp"
#include "texture.hpp"
#include <GL/glew.h>
#include "memory"
#include <string>

enum class PlayerMovement { None, MoveLeft, MoveRight, MoveUp, MoveDown };
enum class PlayerFacing { Left, Up, Right, Down };

struct MovingResult {
    bool needToRefreshTexture;
};

class GLPlayer {
public:
    GLPlayer();
    GLuint vao;
    GLuint vboPosition;
    GLuint vboColor;
    GLuint vboTexture;
    unsigned int glTextureId;
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    const Texture &getTexture() const;
    Point<> getGridPosition() const;
    Point<float> getGLObjectPositionWithMovement() const;
    bool isInMovement() const;
    bool isRunning() const;
    bool isFacing(PlayerFacing direction);
    void initialize();
    void generateGLPlayerObject(float tileHalfWidth, float tileHalfHeight);
    void unloadGLPlayerObject();
    void setTexture(const TextureInfo &textureInfo);
    void applyCurrentGLTexture(const GLTextureService &textureService);
    void setGridPosition(Point<> position);
    void setGLObjectPosition(float tileHalfWidth, float tileHalfHeight);
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
private:
    Point<> m_coord;
    float m_xMove;
    float m_yMove;
    PlayerMovement m_playerMovement;
    PlayerFacing m_playerFacing;
    bool m_isInClimbingMode;
    bool m_isInRunningMode;
    std::string m_textureName;
    int m_baseTextureIndex;
    int m_currentMovementTextureIndex;
    std::shared_ptr<Texture> m_texture;
};


