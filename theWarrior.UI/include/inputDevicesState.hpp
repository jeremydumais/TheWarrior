#pragma once

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <optional>

namespace thewarrior::ui {

enum class InputElementState
{
    Idle,
    Pressed,
    Released
};

class InputDevicesState
{
public:
    virtual ~InputDevicesState() = default;
    bool getUpPressed() const;
    std::optional<Uint64> getUpPressedTicks() const;
    bool getDownPressed() const;
    std::optional<Uint64> getDownPressedTicks() const;
    bool getLeftPressed() const;
    std::optional<Uint64> getLeftPressedTicks() const;
    bool getRightPressed() const;
    std::optional<Uint64> getRightPressedTicks() const;
    InputElementState getButtonAState() const;
    InputElementState getButtonBState() const;
    InputElementState getButtonCState() const;
    InputElementState getButtonDState() const;
    bool isADirectionKeyPressed() const;
    InputElementState getKeyShiftState() const;
    void reset();
    void processJoystick(SDL_Joystick *joystick);
    InputElementState getElementState(bool pressed, bool previouslyPressed) const;
    void processEvent(SDL_Event &e);
protected:
    void setUpPressed(bool value);
    void setDownPressed(bool value);
    void setLeftPressed(bool value);
    void setRightPressed(bool value);
    void setButtonAState(InputElementState state);
    void setButtonBState(InputElementState state);
    void setButtonCState(InputElementState state);
    void setButtonDState(InputElementState state);
    void setKeyShiftState(InputElementState state);
    virtual Uint64 getTicks() const;
private:
    std::optional<Uint64> m_upPressed = std::nullopt;
    std::optional<Uint64> m_downPressed = std::nullopt;
    std::optional<Uint64> m_leftPressed = std::nullopt;
    std::optional<Uint64> m_rightPressed = std::nullopt;
    InputElementState m_buttonAState = InputElementState::Idle;
    InputElementState m_buttonBState = InputElementState::Idle;
    InputElementState m_buttonCState = InputElementState::Idle;
    InputElementState m_buttonDState = InputElementState::Idle;
    InputElementState m_keyShiftState = InputElementState::Idle;
};

} // namespace thewarrior::ui
