#include <gtest/gtest.h>
#include "shakingAnimation.hpp"

using namespace thewarrior::ui;

TEST(ShakingAnimation_process, With0_5And3_ReturnSuccess) {
    ShakingAnimation animation(0.5F, 3);
    ASSERT_FLOAT_EQ(0.0F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(0.5F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(0.0F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(-0.5F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(0.0F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(1.0F/3.0F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(0.0F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(-1.0F/3.0F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(0.0F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(1.0F/6.0F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(0.0F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(-1.0F/6.0F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(0.0F, animation.getValue());
    ASSERT_TRUE(animation.isCompleted());
}
