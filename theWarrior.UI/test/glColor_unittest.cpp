#include "glColor.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <glm/fwd.hpp>

using thewarrior::ui::GLColor;
using thewarrior::ui::getVec3FromGLColor;
using thewarrior::ui::getVec3FromRGBString;

TEST(getVec3FromGLColor, WithWhiteColor_Return1F1F1F) {
    auto expected = glm::vec3(1.0F, 1.0F, 1.0F);
    ASSERT_EQ(expected, getVec3FromGLColor(GLColor::White));
}

TEST(getVec3FromGLColor, WithGrayColor_Return03F03F03F) {
    auto expected = glm::vec3(0.3F, 0.3F, 0.3F);
    ASSERT_EQ(expected, getVec3FromGLColor(GLColor::Gray));
}

TEST(getVec3FromGLColor, WithLightGrayColor_Return06F06F06F) {
    auto expected = glm::vec3(0.6F, 0.6F, 0.6F);
    ASSERT_EQ(expected, getVec3FromGLColor(GLColor::LightGray));
}

TEST(getVec3FromGLColor, WithGreenColor_Return0F1F0F) {
    auto expected = glm::vec3(0.0F, 1.0F, 0.0F);
    ASSERT_EQ(expected, getVec3FromGLColor(GLColor::Green));
}

TEST(getVec3FromGLColor, WithRedColor_Return1F0F0F) {
    auto expected = glm::vec3(1.0F, 0.0F, 0.0F);
    ASSERT_EQ(expected, getVec3FromGLColor(GLColor::Red));
}

TEST(getVec3FromGLColor, WithAnythingElseColor_Return0F0F0F) {
    auto expected = glm::vec3(0.0F, 0.0F, 0.0F);
    ASSERT_EQ(expected, getVec3FromGLColor(static_cast<GLColor>(444)));
}

TEST(getVec3FromRGBString, WithEmptyValue_ThrowInvalidArgument) {
    try {
        getVec3FromRGBString("");
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("Invalid hexadecimal color string", err.what());
    }
}

TEST(getVec3FromRGBString, WithLettersOutOfHex_Return0Everywhere) {
    const auto actual = getVec3FromRGBString("#GGIIJJ");
    ASSERT_EQ(glm::vec3(0.0F, 0.0F, 0.0F), actual);
}

TEST(getVec3FromRGBString, WithBlackHex_ReturnBlackVec3) {
    const auto actual = getVec3FromRGBString("#000000");
    ASSERT_EQ(glm::vec3(0.0F, 0.0F, 0.0F), actual);
}

TEST(getVec3FromRGBString, WithWhiteHex_ReturnWhiteVec3) {
    const auto actual = getVec3FromRGBString("#FFFFFF");
    ASSERT_EQ(glm::vec3(1.0F, 1.0F, 1.0F), actual);
}

TEST(getVec3FromRGBString, WithGreenHex_ReturnGreenVec3) {
    const auto actual = getVec3FromRGBString("#00FF00");
    ASSERT_EQ(glm::vec3(0.0F, 1.0F, 0.0F), actual);
}
