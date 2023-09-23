#include "inputDevicesState.hpp"
#include <SDL2/SDL_timer.h>
#include <iostream>

namespace thewarrior::ui {

bool InputDevicesState::getUpPressed() const
{
    return m_upPressed.has_value();
}

std::optional<Uint64> InputDevicesState::getUpPressedTicks() const
{
    return m_upPressed.has_value() ? std::optional<Uint64>(m_upPressed.value()) : std::nullopt;
}

bool InputDevicesState::getDownPressed() const
{
    return m_downPressed.has_value();
}

std::optional<Uint64> InputDevicesState::getDownPressedTicks() const
{
    return m_downPressed.has_value() ? std::optional<Uint64>(m_downPressed.value()) : std::nullopt;
}

bool InputDevicesState::getLeftPressed() const
{
    return m_leftPressed.has_value();
}

std::optional<Uint64> InputDevicesState::getLeftPressedTicks() const
{
    return m_leftPressed.has_value() ? std::optional<Uint64>(m_leftPressed.value()) : std::nullopt;
}

bool InputDevicesState::getRightPressed() const
{
    return m_rightPressed.has_value();
}

std::optional<Uint64> InputDevicesState::getRightPressedTicks() const
{
    return m_rightPressed.has_value() ? std::optional<Uint64>(m_rightPressed.value()) : std::nullopt;
}

InputElementState InputDevicesState::getButtonAState() const
{
    return m_buttonAState;
}

InputElementState InputDevicesState::getButtonBState() const
{
    return m_buttonBState;
}

InputElementState InputDevicesState::getButtonCState() const
{
    return m_buttonCState;
}

InputElementState InputDevicesState::getButtonDState() const
{
    return m_buttonDState;
}

bool InputDevicesState::isADirectionKeyPressed() const
{
    return m_upPressed || m_downPressed || m_leftPressed || m_rightPressed;
}

InputElementState InputDevicesState::getKeyShiftState() const
{
    return m_keyShiftState;
}

void InputDevicesState::reset()
{
    m_upPressed = std::nullopt;
    m_downPressed = std::nullopt;
    m_leftPressed = std::nullopt;
    m_rightPressed = std::nullopt;
    m_buttonAState = InputElementState::Idle;
    m_buttonBState = InputElementState::Idle;
    m_buttonCState = InputElementState::Idle;
    m_buttonDState = InputElementState::Idle;
    m_keyShiftState = InputElementState::Idle;
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
    auto buttonAPreviousState = m_buttonAState;
    auto buttonBPreviousState = m_buttonBState;
    auto buttonCPreviousState = m_buttonCState;
    auto buttonDPreviousState = m_buttonDState;
    reset();
    setButtonAState(getElementState(buttonAPressed, buttonAPreviousState == InputElementState::Pressed));
    setButtonBState(getElementState(buttonBPressed, buttonBPreviousState == InputElementState::Pressed));
    setButtonCState(getElementState(buttonCPressed, buttonCPreviousState == InputElementState::Pressed));
    setButtonDState(getElementState(buttonDPressed, buttonDPreviousState == InputElementState::Pressed));
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

InputElementState InputDevicesState::getElementState(bool pressed, bool previouslyPressed) const
{
    if (pressed) {
        return InputElementState::Pressed;
    }
    else if (!pressed && previouslyPressed) {
        return InputElementState::Released;
    }
    else {
        return InputElementState::Idle;
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
        setButtonAState(InputElementState::Released);
    }
    if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE) {
        setButtonBState(InputElementState::Released);
    }

    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    if(keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT]) {
        setKeyShiftState(InputElementState::Pressed);
    }
    else {
        setKeyShiftState(InputElementState::Idle);
    }
}

void InputDevicesState::setUpPressed(bool value)
{
    if (!value) {
        m_upPressed = std::nullopt;
        return;
    }
    if (!m_upPressed.has_value()) {
        m_upPressed = getTicks();
    }
}

void InputDevicesState::setDownPressed(bool value)
{
    if (!value) {
        m_downPressed = std::nullopt;
        return;
    }
    if (!m_downPressed.has_value()) {
        m_downPressed = getTicks();
    }
}

void InputDevicesState::setLeftPressed(bool value)
{
    if (!value) {
        m_leftPressed = std::nullopt;
        return;
    }
    if (!m_leftPressed.has_value()) {
        m_leftPressed = getTicks();
    }
}

void InputDevicesState::setRightPressed(bool value)
{
    if (!value) {
        m_rightPressed = std::nullopt;
        return;
    }
    if (!m_rightPressed.has_value()) {
        m_rightPressed = getTicks();
    }
}

void InputDevicesState::setButtonAState(InputElementState state)
{
    m_buttonAState = state;
}

void InputDevicesState::setButtonBState(InputElementState state)
{
    m_buttonBState = state;
}

void InputDevicesState::setButtonCState(InputElementState state)
{
    m_buttonCState = state;
}

void InputDevicesState::setButtonDState(InputElementState state)
{
    m_buttonDState = state;
}

void InputDevicesState::setKeyShiftState(InputElementState state)
{
    m_keyShiftState = state;
}

Uint64 InputDevicesState::getTicks() const
{
    return SDL_GetTicks64();
}

} // namespace thewarrior::ui
