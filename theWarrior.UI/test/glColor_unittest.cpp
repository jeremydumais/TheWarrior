#include "glColor.hpp"
#include <gtest/gtest.h>

TEST(getVec3FromGLColor, WithWhiteColor_Return1F1F1F)
{
    auto expected = glm::vec3(1.0F, 1.0F, 1.0F);
    ASSERT_EQ(expected, getVec3FromGLColor(GLColor::White));    
}

TEST(getVec3FromGLColor, WithGrayColor_Return03F03F03F)
{
    auto expected = glm::vec3(0.3F, 0.3F, 0.3F);
    ASSERT_EQ(expected, getVec3FromGLColor(GLColor::Gray));    
}

TEST(getVec3FromGLColor, WithLightGrayColor_Return06F06F06F)
{
    auto expected = glm::vec3(0.6F, 0.6F, 0.6F);
    ASSERT_EQ(expected, getVec3FromGLColor(GLColor::LightGray));    
}

TEST(getVec3FromGLColor, WithGreenColor_Return0F1F0F)
{
    auto expected = glm::vec3(0.0F, 1.0F, 0.0F);
    ASSERT_EQ(expected, getVec3FromGLColor(GLColor::Green));    
}

TEST(getVec3FromGLColor, WithRedColor_Return1F0F0F)
{
    auto expected = glm::vec3(1.0F, 0.0F, 0.0F);
    ASSERT_EQ(expected, getVec3FromGLColor(GLColor::Red));    
}

TEST(getVec3FromGLColor, WithAnythingElseColor_Return0F0F0F)
{
    auto expected = glm::vec3(0.0F, 0.0F, 0.0F);
    ASSERT_EQ(expected, getVec3FromGLColor(static_cast<GLColor>(444)));    
}
