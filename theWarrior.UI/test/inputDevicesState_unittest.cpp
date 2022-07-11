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
    void setButtonAPre(bool value) { setButtonAPressed(value); }
    void setButtonBPre(bool value) { setButtonBPressed(value); }
    void setButtonCPre(bool value) { setButtonCPressed(value); }
    void setButtonDPre(bool value) { setButtonDPressed(value); }
    void setButtonARel(bool value) { setButtonAReleased(value); }
    void setButtonBRel(bool value) { setButtonBReleased(value); }
    void setButtonCRel(bool value) { setButtonCReleased(value); }
    void setButtonDRel(bool value) { setButtonDReleased(value); }
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

TEST(InputDevicesState_getButtonAReleased, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getButtonAReleased());
}

TEST(InputDevicesState_getButtonBReleased, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getButtonBReleased());
}

TEST(InputDevicesState_getButtonCReleased, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getButtonCReleased());
}

TEST(InputDevicesState_getButtonDReleased, WithDefaultConstructed_ReturnFalse)
{
    InputDevicesState devicesState;
    ASSERT_FALSE(devicesState.getButtonDReleased());
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
    setButtonAPre(true);
    ASSERT_TRUE(getButtonAPressed());
}

TEST_F(InputDevicesStateWithSetters, setButtonBPressed_WithTrue_ReturnSuccess)
{
    setButtonBPre(true);
    ASSERT_TRUE(getButtonBPressed());
}

TEST_F(InputDevicesStateWithSetters, setButtonCPressed_WithTrue_ReturnSuccess)
{
    setButtonCPre(true);
    ASSERT_TRUE(getButtonCPressed());
}

TEST_F(InputDevicesStateWithSetters, setButtonDPressed_WithTrue_ReturnSuccess)
{
    setButtonDPre(true);
    ASSERT_TRUE(getButtonDPressed());
}

TEST_F(InputDevicesStateWithSetters, setButtonAReleased_WithTrue_ReturnSuccess)
{
    setButtonARel(true);
    ASSERT_TRUE(getButtonAReleased());
}

TEST_F(InputDevicesStateWithSetters, setButtonBReleased_WithTrue_ReturnSuccess)
{
    setButtonBRel(true);
    ASSERT_TRUE(getButtonBReleased());
}

TEST_F(InputDevicesStateWithSetters, setButtonCReleased_WithTrue_ReturnSuccess)
{
    setButtonCRel(true);
    ASSERT_TRUE(getButtonCReleased());
}

TEST_F(InputDevicesStateWithSetters, setButtonDReleased_WithTrue_ReturnSuccess)
{
    setButtonDRel(true);
    ASSERT_TRUE(getButtonDReleased());
}

TEST_F(InputDevicesStateWithSetters, reset_WithSettingAllTrueBefore_ReturnSuccess)
{
    setUp(true);
    setDown(true);
    setLeft(true);
    setRight(true);
    setButtonAPre(true);
    setButtonBPre(true);
    setButtonCPre(true);
    setButtonDPre(true);
    setButtonARel(true);
    setButtonBRel(true);
    setButtonCRel(true);
    setButtonDRel(true);
    reset();
    ASSERT_FALSE(getUpPressed());
    ASSERT_FALSE(getDownPressed());
    ASSERT_FALSE(getLeftPressed());
    ASSERT_FALSE(getRightPressed());
    ASSERT_FALSE(getButtonAPressed());
    ASSERT_FALSE(getButtonBPressed());
    ASSERT_FALSE(getButtonCPressed());
    ASSERT_FALSE(getButtonDPressed());
    ASSERT_FALSE(getButtonAReleased());
    ASSERT_FALSE(getButtonBReleased());
    ASSERT_FALSE(getButtonCReleased());
    ASSERT_FALSE(getButtonDReleased());
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