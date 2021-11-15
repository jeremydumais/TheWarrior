#pragma once

#include "point.hpp"
#include <GL/glew.h>
#include <string>

enum class PlayerMovement { None, MoveLeft, MoveRight, MoveUp, MoveDown };
enum class PlayerFacing { Left, Up, Right, Down };

struct MovingResult {
    bool needToRefreshTexture;
};

class GLPlayer {
public:
    GLPlayer();
    Point coord;
    GLuint vao;
    GLuint vboPosition;
    GLuint vboColor;
    GLuint vboTexture;
    float xMove;
    float yMove;
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    bool isInMovement() const;
    bool isRunning() const;
    bool isFacing(PlayerFacing direction);
    void initialize();
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
    PlayerMovement m_playerMovement;
    PlayerFacing m_playerFacing;
    bool m_isInClimbingMode;
    bool m_isInRunningMode;
    std::string m_textureName;
    int m_baseTextureIndex;
    int m_currentMovementTextureIndex;
};


