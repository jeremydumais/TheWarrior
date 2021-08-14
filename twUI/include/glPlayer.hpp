#pragma once

#include "point.hpp"
#include <GL/glew.h>
#include <string>

enum class PlayerMovement { None, MoveLeft, MoveRight, MoveUp, MoveDown };

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
    MovingResult processMoving(double delta_time);
private:
    PlayerMovement playerMovement;
    bool isInClimbingMode;
    bool isInRunningMode;
    std::string textureName;
    int baseTextureIndex;
    int currentMovementTextureIndex;
};
