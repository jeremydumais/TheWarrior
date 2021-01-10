#include "glPlayer.hpp"

using namespace std;

const string& GLPlayer::getTextureName() const
{
    return textureName;
}

int GLPlayer::getTextureIndex() const
{
    return currentMovementTextureIndex;
}

bool GLPlayer::isInMovement() const
{
    return playerMovement != PlayerMovement::None;
}

void GLPlayer::initialize() 
{
    textureName = "NPC1";
    x = 7;
    y = 14;
    xMove = 0.0f;
    yMove = 0.0f;
    baseTextureIndex = 9;
    currentMovementTextureIndex = baseTextureIndex + 1;
    playerMovement = PlayerMovement::None;
}

void GLPlayer::moveUp() 
{
    playerMovement = PlayerMovement::MoveUp;
    y -= 1;
    yMove = 1.0f;
    currentMovementTextureIndex = baseTextureIndex;
}

void GLPlayer::moveDown() 
{
    playerMovement = PlayerMovement::MoveDown;
    y += 1;
    yMove = -1.0f;
    currentMovementTextureIndex = baseTextureIndex + 36;
}

void GLPlayer::moveLeft() 
{
    playerMovement = PlayerMovement::MoveLeft;
    x -= 1;
    xMove = 1.0f;
    currentMovementTextureIndex = baseTextureIndex + 24;
}

void GLPlayer::moveRight() 
{
    playerMovement = PlayerMovement::MoveRight;
    x += 1;
    xMove = -1.0f;
    currentMovementTextureIndex = baseTextureIndex + 12;
}
void GLPlayer::faceUp() 
{
    currentMovementTextureIndex = baseTextureIndex + 1;
}

void GLPlayer::faceDown() 
{
    currentMovementTextureIndex = baseTextureIndex + 37;
}

void GLPlayer::faceLeft() 
{
    currentMovementTextureIndex = baseTextureIndex + 25;
}

void GLPlayer::faceRight() 
{
    currentMovementTextureIndex = baseTextureIndex + 13;
}

MovingResult GLPlayer::processMoving(double delta_time) 
{
    MovingResult result;
    const float SPEED = 8.0f;
    if (playerMovement == PlayerMovement::MoveUp) {
        yMove -= SPEED * delta_time;
        if (yMove < 0.0f) {
            yMove = 0.0f;
            playerMovement = PlayerMovement::None;
        }
        else if(yMove < 0.3f) {
            if (currentMovementTextureIndex != 1) {
                currentMovementTextureIndex = baseTextureIndex + 1;
                result.needToRefreshTexture = true;
            }
        }
        else if(yMove < 0.6f) {
            if (currentMovementTextureIndex != 2) {
                currentMovementTextureIndex = baseTextureIndex + 2;
                result.needToRefreshTexture = true;
            }
        }
    }
    else if (playerMovement == PlayerMovement::MoveDown) {
        yMove += SPEED * delta_time;
        if (yMove > 0.0f) {
            yMove = 0.0f;
            playerMovement = PlayerMovement::None;
        }
        else if(yMove > -0.3f) {
            if (currentMovementTextureIndex != baseTextureIndex + 37) {
                currentMovementTextureIndex = baseTextureIndex + 37;
                result.needToRefreshTexture = true;
            }
        }
        else if(yMove > -0.6f) {
            if (currentMovementTextureIndex != baseTextureIndex + 38) {
                currentMovementTextureIndex = baseTextureIndex + 38;
                result.needToRefreshTexture = true;
            }
        }
    }
    else if (playerMovement == PlayerMovement::MoveLeft) {
        xMove -= SPEED * delta_time;
        if (xMove < 0.0f) {
            xMove = 0.0f;
            playerMovement = PlayerMovement::None;
        }
        else if(xMove < 0.3f) {
            if (currentMovementTextureIndex != baseTextureIndex + 25) {
                currentMovementTextureIndex = baseTextureIndex + 25;
                result.needToRefreshTexture = true;
            }
        }
        else if(xMove < 0.6f) {
            if (currentMovementTextureIndex != baseTextureIndex + 26) {
                currentMovementTextureIndex = baseTextureIndex + 26;
                result.needToRefreshTexture = true;
            }
        }
    }    
    else if (playerMovement == PlayerMovement::MoveRight) {
        xMove += SPEED * delta_time;
        if (xMove > 0.0f) {
            xMove = 0.0f;
            playerMovement = PlayerMovement::None;
        }
        else if(xMove > -0.3f) {
            if (currentMovementTextureIndex != baseTextureIndex + 13) {
                currentMovementTextureIndex = baseTextureIndex + 13;
                result.needToRefreshTexture = true;
            }
        }
        else if(xMove > -0.6f) {
            if (currentMovementTextureIndex != baseTextureIndex + 14) {
                currentMovementTextureIndex = baseTextureIndex + 14;
                result.needToRefreshTexture = true;
            }
        }
    }
}
