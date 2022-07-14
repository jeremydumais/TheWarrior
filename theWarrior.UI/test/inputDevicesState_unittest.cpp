#include "inputDevicesState.hpp"
#include <gtest/gtest.h>

class InputDevicesStateWithSetters : public InputDevicesState, 
                                     public ::testing::Test
{
public:
    void setUp(bool value) { InputDevicesState::setUpPressed(value); }
    void setDown(bool value) { InputDevicesState::setDownPressed(value); }
    void setLeft(bool value) { InputDevicesState::setLeftPressed(value); }
    void setRight(bool value) { InputDevicesState::setRightPressed(value); }
    void setButtonASt(InputElementState state) { setButtonAState(state); }
    void setButtonBSt(InputElementState state) { setButtonBState(state); }
    void setButtonCSt(InputElementState state) { setButtonCState(state); }
    void setButtonDSt(InputElementState state) { setButtonDState(state); }
    void setKeyShiftSt(InputElementState state) { setKeyShiftState(state); }
    Uint64 getTicks() const override { return tickCountSample; }
    void setTicks(Uint64 ticks) { tickCountSample = ticks; }
    Uint64 tickCountSample = 12345;
};

TEST(InputDevicesState_getUpPressed, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getUpPressed());
}

TEST_F(InputDevicesStateWithSetters, getUpPressedTicks_WithNoSet_ReturnNullOpt)
{
    ASSERT_EQ(std::nullopt, getUpPressedTicks());
}

TEST_F(InputDevicesStateWithSetters, getUpPressedTicks_WithSetOnce_ReturnNullOpt)
{
    setUp(true);
    ASSERT_EQ(tickCountSample, getUpPressedTicks());
}

TEST_F(InputDevicesStateWithSetters, getUpPressedTicks_WithSetTwice_ReturnNullOpt)
{
    setUp(true);
    const Uint64 initialTicks = getTicks();
    setTicks(45678);
    setUp(true);
    ASSERT_EQ(initialTicks, getUpPressedTicks());
}

TEST(InputDevicesState_getDownPressed, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getDownPressed());
}

TEST_F(InputDevicesStateWithSetters, getDownPressedTicks_WithNoSet_ReturnNullOpt)
{
    ASSERT_EQ(std::nullopt, getDownPressedTicks());
}

TEST_F(InputDevicesStateWithSetters, getDownPressedTicks_WithSetOnce_ReturnNullOpt)
{
    setDown(true);
    ASSERT_EQ(tickCountSample, getDownPressedTicks());
}

TEST_F(InputDevicesStateWithSetters, getDownPressedTicks_WithSetTwice_ReturnNullOpt)
{
    setDown(true);
    const Uint64 initialTicks = getTicks();
    setTicks(45678);
    setDown(true);
    ASSERT_EQ(initialTicks, getDownPressedTicks());
}

TEST(InputDevicesState_getLeftPressed, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getLeftPressed());
}

TEST_F(InputDevicesStateWithSetters, getLeftPressedTicks_WithNoSet_ReturnNullOpt)
{
    ASSERT_EQ(std::nullopt, getLeftPressedTicks());
}

TEST_F(InputDevicesStateWithSetters, getLeftPressedTicks_WithSetOnce_ReturnNullOpt)
{
    setLeft(true);
    ASSERT_EQ(tickCountSample, getLeftPressedTicks());
}

TEST_F(InputDevicesStateWithSetters, getLeftPressedTicks_WithSetTwice_ReturnNullOpt)
{
    setLeft(true);
    const Uint64 initialTicks = getTicks();
    setTicks(45678);
    setLeft(true);
    ASSERT_EQ(initialTicks, getLeftPressedTicks());
}

TEST(InputDevicesState_getRightPressed, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getRightPressed());
}

TEST_F(InputDevicesStateWithSetters, getRightPressedTicks_WithNoSet_ReturnNullOpt)
{
    ASSERT_EQ(std::nullopt, getRightPressedTicks());
}

TEST_F(InputDevicesStateWithSetters, getRightPressedTicks_WithSetOnce_ReturnNullOpt)
{
    setRight(true);
    ASSERT_EQ(tickCountSample, getRightPressedTicks());
}

TEST_F(InputDevicesStateWithSetters, getRightPressedTicks_WithSetTwice_ReturnNullOpt)
{
    setRight(true);
    const Uint64 initialTicks = getTicks();
    setTicks(45678);
    setRight(true);
    ASSERT_EQ(initialTicks, getRightPressedTicks());
}

TEST(InputDevicesState_getButtonAState, WithDefaultConstructed_ReturnIdle)
{
    InputDevicesState devicesState;
    ASSERT_EQ(InputElementState::Idle, devicesState.getButtonAState());
}

TEST(InputDevicesState_getButtonBState, WithDefaultConstructed_ReturnIdle)
{
    InputDevicesState devicesState;
    ASSERT_EQ(InputElementState::Idle, devicesState.getButtonBState());
}

TEST(InputDevicesState_getButtonCState, WithDefaultConstructed_ReturnIdle)
{
    InputDevicesState devicesState;
    ASSERT_EQ(InputElementState::Idle, devicesState.getButtonCState());
}

TEST(InputDevicesState_getButtonDState, WithDefaultConstructed_ReturnIdle)
{
    InputDevicesState devicesState;
    ASSERT_EQ(InputElementState::Idle, devicesState.getButtonDState());
}

TEST(InputDevicesState_getKeyShiftState, WithDefaultConstructed_ReturnIdle)
{
    InputDevicesState devicesState;
    ASSERT_EQ(InputElementState::Idle, devicesState.getKeyShiftState());
}

TEST_F(InputDevicesStateWithSetters, setUpPressed_WithTrue_ReturnSuccess)
{
    setUp(true);
    ASSERT_TRUE(getUpPressed());
}

TEST_F(InputDevicesStateWithSetters, setDownPressed_WithTrue_ReturnSuccess)
{
    setDown(true);
    ASSERT_TRUE(getDownPressed());
}

TEST_F(InputDevicesStateWithSetters, setLeftPressed_WithTrue_ReturnSuccess)
{
    setLeft(true);
    ASSERT_TRUE(getLeftPressed());
}

TEST_F(InputDevicesStateWithSetters, setRightPressed_WithTrue_ReturnSuccess)
{
    setRight(true);
    ASSERT_TRUE(getRightPressed());
}

TEST_F(InputDevicesStateWithSetters, setButtonAState_WithReleased_ReturnSuccess)
{
    setButtonASt(InputElementState::Released);
    ASSERT_EQ(InputElementState::Released, getButtonAState());
}

TEST_F(InputDevicesStateWithSetters, setButtonAState_WithPressed_ReturnSuccess)
{
    setButtonASt(InputElementState::Pressed);
    ASSERT_EQ(InputElementState::Pressed, getButtonAState());
}

TEST_F(InputDevicesStateWithSetters, setButtonBState_WithReleased_ReturnSuccess)
{
    setButtonBSt(InputElementState::Released);
    ASSERT_EQ(InputElementState::Released, getButtonBState());
}

TEST_F(InputDevicesStateWithSetters, setButtonBState_WithPressed_ReturnSuccess)
{
    setButtonBSt(InputElementState::Pressed);
    ASSERT_EQ(InputElementState::Pressed, getButtonBState());
}

TEST_F(InputDevicesStateWithSetters, setButtonCState_WithReleased_ReturnSuccess)
{
    setButtonCSt(InputElementState::Released);
    ASSERT_EQ(InputElementState::Released, getButtonCState());
}

TEST_F(InputDevicesStateWithSetters, setButtonCState_WithPressed_ReturnSuccess)
{
    setButtonCSt(InputElementState::Pressed);
    ASSERT_EQ(InputElementState::Pressed, getButtonCState());
}

TEST_F(InputDevicesStateWithSetters, setButtonDState_WithReleased_ReturnSuccess)
{
    setButtonDSt(InputElementState::Released);
    ASSERT_EQ(InputElementState::Released, getButtonDState());
}

TEST_F(InputDevicesStateWithSetters, setButtonDState_WithPressed_ReturnSuccess)
{
    setButtonDSt(InputElementState::Pressed);
    ASSERT_EQ(InputElementState::Pressed, getButtonDState());
}

TEST_F(InputDevicesStateWithSetters, reset_WithSettingAllTrueBefore_ReturnSuccess)
{
    setUp(true);
    setDown(true);
    setLeft(true);
    setRight(true);
    setButtonASt(InputElementState::Pressed);
    setButtonBSt(InputElementState::Pressed);
    setButtonCSt(InputElementState::Pressed);
    setButtonDSt(InputElementState::Pressed);
    setKeyShiftSt(InputElementState::Pressed);
    reset();
    ASSERT_FALSE(getUpPressed());
    ASSERT_FALSE(getDownPressed());
    ASSERT_FALSE(getLeftPressed());
    ASSERT_FALSE(getRightPressed());
    ASSERT_EQ(InputElementState::Idle, getButtonAState());
    ASSERT_EQ(InputElementState::Idle, getButtonBState());
    ASSERT_EQ(InputElementState::Idle, getButtonCState());
    ASSERT_EQ(InputElementState::Idle, getButtonDState());
    ASSERT_EQ(InputElementState::Idle, getKeyShiftState());
}

TEST_F(InputDevicesStateWithSetters, isADirectionKeyPressed_WithUpPressed_ReturnTrue)
{
    setUp(true);
    ASSERT_TRUE(isADirectionKeyPressed());
}

TEST_F(InputDevicesStateWithSetters, isADirectionKeyPressed_WithDownPressed_ReturnTrue)
{
    setDown(true);
    ASSERT_TRUE(isADirectionKeyPressed());
}

TEST_F(InputDevicesStateWithSetters, isADirectionKeyPressed_WithLeftPressed_ReturnTrue)
{
    setLeft(true);
    ASSERT_TRUE(isADirectionKeyPressed());
}

TEST_F(InputDevicesStateWithSetters, isADirectionKeyPressed_WithRightPressed_ReturnTrue)
{
    setRight(true);
    ASSERT_TRUE(isADirectionKeyPressed());
}

TEST_F(InputDevicesStateWithSetters, isADirectionKeyPressed_WithNoDirectionKeyPressed_ReturnFalse)
{
    ASSERT_FALSE(isADirectionKeyPressed());
}

TEST_F(InputDevicesStateWithSetters, setKeyShiftState_WithReleased_ReturnSuccess)
{
    setKeyShiftSt(InputElementState::Released);
    ASSERT_EQ(InputElementState::Released, getKeyShiftState());
}

TEST_F(InputDevicesStateWithSetters, setKeyShiftState_WithPressed_ReturnSuccess)
{
    setKeyShiftSt(InputElementState::Pressed);
    ASSERT_EQ(InputElementState::Pressed, getKeyShiftState());
}
 