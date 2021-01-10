#pragma once

#include <GL/glew.h>
#include <string>

enum class PlayerMovement { None, MoveLeft, MoveRight, MoveUp, MoveDown };

struct MovingResult {
    bool needToRefreshTexture;
};

class GLPlayer {
public:
    int x;
    int y;
    GLuint vao;
    GLuint vboPosition;
    GLuint vboColor;
    GLuint vboTexture;
    float xMove;
    float yMove;
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    bool isInMovement() const;
    void initialize();
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void faceUp();
    void faceDown();
    void faceLeft();
    void faceRight();
    MovingResult processMoving(double delta_time);
private:
    PlayerMovement playerMovement;
    std::string textureName;
    int baseTextureIndex;
    int currentMovementTextureIndex;
};
