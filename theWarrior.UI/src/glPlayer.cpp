#include "glPlayer.hpp"

using namespace std;


GLPlayer::GLPlayer()
    : coord(0, 0),
      m_isInClimbingMode(false),
      m_isInRunningMode(false)
{
}

const string& GLPlayer::getTextureName() const
{
    return m_textureName;
}

int GLPlayer::getTextureIndex() const
{
    return m_currentMovementTextureIndex;
}

bool GLPlayer::isInMovement() const
{
    return m_playerMovement != PlayerMovement::None;
}

bool GLPlayer::isRunning() const
{
    return m_isInRunningMode;
}

bool GLPlayer::isFacing(PlayerFacing direction) 
{
    return m_playerFacing == direction;
}

void GLPlayer::initialize() 
{
    m_textureName = "NPC1";
    coord = Point(7, 14);
    xMove = 0.0f;
    yMove = 0.0f;
    m_baseTextureIndex = 9;
    m_currentMovementTextureIndex = m_baseTextureIndex + 1;
    m_playerMovement = PlayerMovement::None;
    m_playerFacing = PlayerFacing::Up;
}

void GLPlayer::moveUp() 
{
    m_playerMovement = PlayerMovement::MoveUp;
    coord.setY(coord.y() - 1);
    yMove = 1.0f;
    m_currentMovementTextureIndex = m_baseTextureIndex;
}

void GLPlayer::moveDown(bool isInClimbingMode) 
{
    m_playerMovement = PlayerMovement::MoveDown;
    this->m_isInClimbingMode = isInClimbingMode;
    coord.setY(coord.y() + 1);
    yMove = -1.0f;
    if (isInClimbingMode) {
        //Face up
        m_currentMovementTextureIndex = m_baseTextureIndex;
    }
    else {
        m_currentMovementTextureIndex = m_baseTextureIndex + 36;
    }
}

void GLPlayer::moveLeft() 
{
    m_playerMovement = PlayerMovement::MoveLeft;
    coord.setX(coord.x() - 1);
    xMove = 1.0f;
    m_currentMovementTextureIndex = m_baseTextureIndex + 24;
}

void GLPlayer::moveRight() 
{
    m_playerMovement = PlayerMovement::MoveRight;
    coord.setX(coord.x() + 1);
    xMove = -1.0f;
    m_currentMovementTextureIndex = m_baseTextureIndex + 12;
}
void GLPlayer::faceUp() 
{
    m_currentMovementTextureIndex = m_baseTextureIndex + 1;
    m_playerFacing = PlayerFacing::Up;
}

void GLPlayer::faceDown() 
{
    m_currentMovementTextureIndex = m_baseTextureIndex + 37;
    m_playerFacing = PlayerFacing::Down;
}

void GLPlayer::faceLeft() 
{
    m_currentMovementTextureIndex = m_baseTextureIndex + 25;
    m_playerFacing = PlayerFacing::Left;
}

void GLPlayer::faceRight() 
{
    m_currentMovementTextureIndex = m_baseTextureIndex + 13;
    m_playerFacing = PlayerFacing::Right;
}

void GLPlayer::enableRunMode() 
{
    m_isInRunningMode = true;
}

void GLPlayer::disableRunMode() 
{
    m_isInRunningMode = false;
}

MovingResult GLPlayer::processMoving(float delta_time) 
{
    MovingResult result;
    const float SPEED = m_isInRunningMode ? 11.0f : 7.0f;
    if (m_playerMovement == PlayerMovement::MoveUp) {
        yMove -= SPEED * delta_time;
        if (yMove < 0.0f) {
            yMove = 0.0f;
            m_playerMovement = PlayerMovement::None;
            m_isInClimbingMode = false;
        }
        else if(yMove < 0.3f) {
            if (m_currentMovementTextureIndex != 1) {
                m_currentMovementTextureIndex = m_baseTextureIndex + 1;
                result.needToRefreshTexture = true;
            }
        }
        else if(yMove < 0.6f) {
            if (m_currentMovementTextureIndex != 2) {
                m_currentMovementTextureIndex = m_baseTextureIndex + 2;
                result.needToRefreshTexture = true;
            }
        }
    }
    else if (m_playerMovement == PlayerMovement::MoveDown) {
        yMove += SPEED * delta_time;
        if (yMove > 0.0f) {
            yMove = 0.0f;
            m_playerMovement = PlayerMovement::None;
            m_isInClimbingMode = false;
        }
        else if(yMove > -0.3f) {
            if (m_isInClimbingMode) {
                if (m_currentMovementTextureIndex != 1) {
                    m_currentMovementTextureIndex = m_baseTextureIndex + 1;
                    result.needToRefreshTexture = true;
                }
            }
            else {
                if (m_currentMovementTextureIndex != m_baseTextureIndex + 37) {
                    m_currentMovementTextureIndex = m_baseTextureIndex + 37;
                    result.needToRefreshTexture = true;
                }
            }
        }
        else if(yMove > -0.6f) {
            if (m_isInClimbingMode) {
                if (m_currentMovementTextureIndex != 2) {
                    m_currentMovementTextureIndex = m_baseTextureIndex + 2;
                    result.needToRefreshTexture = true;
                }
            }
            else {
                if (m_currentMovementTextureIndex != m_baseTextureIndex + 38) {
                    m_currentMovementTextureIndex = m_baseTextureIndex + 38;
                    result.needToRefreshTexture = true;
                }
            }
        }
    }
    else if (m_playerMovement == PlayerMovement::MoveLeft) {
        xMove -= SPEED * delta_time;
        if (xMove < 0.0f) {
            xMove = 0.0f;
            m_playerMovement = PlayerMovement::None;
            m_isInClimbingMode = false;
        }
        else if(xMove < 0.3f) {
            if (m_currentMovementTextureIndex != m_baseTextureIndex + 25) {
                m_currentMovementTextureIndex = m_baseTextureIndex + 25;
                result.needToRefreshTexture = true;
            }
        }
        else if(xMove < 0.6f) {
            if (m_currentMovementTextureIndex != m_baseTextureIndex + 26) {
                m_currentMovementTextureIndex = m_baseTextureIndex + 26;
                result.needToRefreshTexture = true;
            }
        }
    }    
    else if (m_playerMovement == PlayerMovement::MoveRight) {
        xMove += SPEED * delta_time;
        if (xMove > 0.0f) {
            xMove = 0.0f;
            m_playerMovement = PlayerMovement::None;
            m_isInClimbingMode = false;
        }
        else if(xMove > -0.3f) {
            if (m_currentMovementTextureIndex != m_baseTextureIndex + 13) {
                m_currentMovementTextureIndex = m_baseTextureIndex + 13;
                result.needToRefreshTexture = true;
            }
        }
        else if(xMove > -0.6f) {
            if (m_currentMovementTextureIndex != m_baseTextureIndex + 14) {
                m_currentMovementTextureIndex = m_baseTextureIndex + 14;
                result.needToRefreshTexture = true;
            }
        }
    }
    return result;
}
