#include "inputDevicesState.hpp"
#include <iostream>

bool InputDevicesState::getUpPressed() const
{
    return m_upPressed;
}

bool InputDevicesState::getDownPressed() const
{
    return m_downPressed;
}

bool InputDevicesState::getLeftPressed() const
{
    return m_leftPressed;
}

bool InputDevicesState::getRightPressed() const
{
    return m_rightPressed;
}

bool InputDevicesState::getButtonAPressed() const
{
    return m_buttonAPressed;
}
bool InputDevicesState::getButtonBPressed() const
{
    return m_buttonBPressed;
}

bool InputDevicesState::getButtonCPressed() const
{
    return m_buttonCPressed;
}

bool InputDevicesState::getButtonDPressed() const
{
    return m_buttonDPressed;
}

bool InputDevicesState::getButtonAReleased() const
{
    return m_buttonAReleased;
}

bool InputDevicesState::getButtonBReleased() const
{
    return m_buttonBReleased;
}

bool InputDevicesState::getButtonCReleased() const
{
    return m_buttonCReleased;
}

bool InputDevicesState::getButtonDReleased() const
{
    return m_buttonDReleased;
}

bool InputDevicesState::isADirectionKeyPressed() const
{
    return m_upPressed || m_downPressed || m_leftPressed || m_rightPressed;
}

void InputDevicesState::reset()
{
    m_upPressed = false;
    m_downPressed = false;
    m_leftPressed = false;
    m_rightPressed = false;
    m_buttonAPressed = false;
    m_buttonBPressed = false;
    m_buttonCPressed = false;
    m_buttonDPressed = false;
    m_buttonAReleased = false;
    m_buttonBReleased = false;
    m_buttonCReleased = false;
    m_buttonDReleased = false;
}

void InputDevicesState::processJoystick(SDL_Joystick *joystick)
{   
    bool buttonAPressed = false;
    bool buttonBPressed = false;
    bool buttonCPressed = false;
    bool buttonDPressed = false;
    for (int i = 0; i < SDL_JoystickNumButtons(joystick); i++) {
        if (i == 0 && SDL_JoystickGetButton(joystick, i)) {
            buttonBPressed = true;
        }
        if (i == 1 && SDL_JoystickGetButton(joystick, i)) {
            buttonAPressed = true;
        }
        if (i == 2 && SDL_JoystickGetButton(joystick, i)) {
            buttonCPressed = true;
        }
        if (i == 3 && SDL_JoystickGetButton(joystick, i)) {
            buttonDPressed = true;
        }
    }
    bool buttonAReleased = !buttonAPressed && getButtonAPressed();
    bool buttonBReleased = !buttonBPressed && getButtonBPressed();
    bool buttonCReleased = !buttonCPressed && getButtonCPressed();
    bool buttonDReleased = !buttonDPressed && getButtonDPressed();
    reset();
    setButtonAPressed(buttonAPressed);
    setButtonBPressed(buttonBPressed);
    setButtonCPressed(buttonCPressed);
    setButtonDPressed(buttonDPressed);
    setButtonAReleased(buttonAReleased);
    setButtonBReleased(buttonBReleased);
    setButtonCReleased(buttonCReleased);
    setButtonDReleased(buttonDReleased);
    for (int i = 0 ; i < SDL_JoystickNumHats(joystick); i++ ) {
        if (SDL_JoystickGetHat(joystick, i) == SDL_HAT_UP) {
            setUpPressed(true);
            break;
        }
        else if (SDL_JoystickGetHat(joystick, i) == SDL_HAT_DOWN) {
            setDownPressed(true);
            break;
        }
        else if (SDL_JoystickGetHat(joystick, i) == SDL_HAT_LEFT) {
            setLeftPressed(true);
            break;
        }
        else if (SDL_JoystickGetHat(joystick, i) == SDL_HAT_RIGHT) {
            setRightPressed(true);
            break;
        }
    }
}

void InputDevicesState::processEvent(SDL_Event &e)
{
    if(e.type == SDL_KEYDOWN) {
        switch(e.key.keysym.sym) {
            case SDLK_UP:
                setUpPressed(true);
                break;
            case SDLK_DOWN:
                setDownPressed(true);
                break;
            case SDLK_LEFT:
                setLeftPressed(true);
                break;
            case SDLK_RIGHT:
                setRightPressed(true);
                break;
        };
    }
    if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_RETURN) {
        setButtonAPressed(true);
    }
    if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE) {
        setButtonBPressed(true);
    }
    
}

void InputDevicesState::setUpPressed(bool value)
{
    m_upPressed = value;
}

void InputDevicesState::setDownPressed(bool value)
{
    m_downPressed = value;
}

void InputDevicesState::setLeftPressed(bool value)
{
    m_leftPressed = value;
}

void InputDevicesState::setRightPressed(bool value)
{
    m_rightPressed = value;
}

void InputDevicesState::setButtonAPressed(bool value)
{
    m_buttonAPressed = value;
}

void InputDevicesState::setButtonBPressed(bool value)
{
    m_buttonBPressed = value;
}

void InputDevicesState::setButtonCPressed(bool value)
{
    m_buttonCPressed = value;
}

void InputDevicesState::setButtonDPressed(bool value)
{
    m_buttonDPressed = value;
}

void InputDevicesState::setButtonAReleased(bool value)
{
    m_buttonAReleased = value;
}

void InputDevicesState::setButtonBReleased(bool value)
{
    m_buttonBReleased = value;
}

void InputDevicesState::setButtonCReleased(bool value)
{
    m_buttonCReleased = value;
}

void InputDevicesState::setButtonDReleased(bool value)
{
    m_buttonDReleased = value;
}