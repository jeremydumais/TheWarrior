#pragma once

#include <SDL2/SDL_events.h>

class InputDevicesState
{
public:
    virtual ~InputDevicesState() = default;
    bool getUpPressed() const;
    bool getDownPressed() const;
    bool getLeftPressed() const;
    bool getRightPressed() const;
    bool getButtonAPressed() const;
    bool getButtonBPressed() const;
    bool getButtonCPressed() const;
    bool getButtonDPressed() const;
    bool isADirectionKeyPressed() const;
    void reset();
    void processJoystick(SDL_Joystick *joystick);
    void processEvent(SDL_Event &e);
protected:
    void setUpPressed(bool value);
    void setDownPressed(bool value);
    void setLeftPressed(bool value);
    void setRightPressed(bool value);
    void setButtonAPressed(bool value);
    void setButtonBPressed(bool value);
    void setButtonCPressed(bool value);
    void setButtonDPressed(bool value);
private:
    bool m_upPressed = false;
    bool m_downPressed = false;
    bool m_leftPressed = false;
    bool m_rightPressed = false;
    bool m_buttonAPressed = false;
    bool m_buttonBPressed = false;
    bool m_buttonCPressed = false;
    bool m_buttonDPressed = false;
};