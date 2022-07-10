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
    void setButtonA(bool value) { setButtonAPressed(value); }
    void setButtonB(bool value) { setButtonBPressed(value); }
    void setButtonC(bool value) { setButtonCPressed(value); }
    void setButtonD(bool value) { setButtonDPressed(value); }
};

TEST(InputDevicesState_getUpPressed, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getUpPressed());
}

TEST(InputDevicesState_getDownPressed, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getDownPressed());
}

TEST(InputDevicesState_getLeftPressed, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getLeftPressed());
}

TEST(InputDevicesState_getRightPressed, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getRightPressed());
}

TEST(InputDevicesState_getButtonAPressed, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getButtonAPressed());
}

TEST(InputDevicesState_getButtonBPressed, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getButtonBPressed());
}

TEST(InputDevicesState_getButtonCPressed, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getButtonCPressed());
}

TEST(InputDevicesState_getButtonDPressed, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getButtonDPressed());
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

TEST_F(InputDevicesStateWithSetters, setButtonAPressed_WithTrue_ReturnSuccess)
{
    setButtonA(true);
    ASSERT_TRUE(getButtonAPressed());
}

TEST_F(InputDevicesStateWithSetters, setButtonBPressed_WithTrue_ReturnSuccess)
{
    setButtonB(true);
    ASSERT_TRUE(getButtonBPressed());
}

TEST_F(InputDevicesStateWithSetters, setButtonCPressed_WithTrue_ReturnSuccess)
{
    setButtonC(true);
    ASSERT_TRUE(getButtonCPressed());
}

TEST_F(InputDevicesStateWithSetters, setButtonDPressed_WithTrue_ReturnSuccess)
{
    setButtonD(true);
    ASSERT_TRUE(getButtonDPressed());
}

TEST_F(InputDevicesStateWithSetters, reset_WithSettingAllTrueBefore_ReturnSuccess)
{
    setUp(true);
    setDown(true);
    setLeft(true);
    setRight(true);
    setButtonA(true);
    setButtonB(true);
    setButtonC(true);
    setButtonD(true);
    reset();
    ASSERT_FALSE(getUpPressed());
    ASSERT_FALSE(getDownPressed());
    ASSERT_FALSE(getLeftPressed());
    ASSERT_FALSE(getRightPressed());
    ASSERT_FALSE(getButtonAPressed());
    ASSERT_FALSE(getButtonBPressed());
    ASSERT_FALSE(getButtonCPressed());
    ASSERT_FALSE(getButtonDPressed());
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