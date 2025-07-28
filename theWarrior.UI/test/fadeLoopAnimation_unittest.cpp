#include <gtest/gtest.h>
#include "fadeLoopAnimation.hpp"

using namespace thewarrior::ui;

TEST(FadeLoopAnimation_process, With1To0_1_ReturnSuccess) {
    FadeLoopAnimation animation(0.1F, 1.0F, 0.5F);
    ASSERT_FLOAT_EQ(1.0F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(0.5F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(0.1F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(0.6F, animation.getValue());
    animation.process();
    ASSERT_FLOAT_EQ(1.0F, animation.getValue());
}
