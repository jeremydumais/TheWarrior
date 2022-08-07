#include "size.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace thewarrior::models;

TEST(size_Contructor, With1and2_ReturnSuccess)
{
    Size actual(1, 2);
    ASSERT_EQ(1, actual.width());
    ASSERT_EQ(2, actual.height());
}

TEST(size_x, With1_Return1)
{
    Size<int> actual(1, 2);
    ASSERT_EQ(1, actual.width());
}

TEST(size_y, With2_Return2)
{
    Size<int> actual(1, 2);
    ASSERT_EQ(2, actual.height());
}

TEST(size_setX, With3_ReturnSuccess)
{
    Size<int> actual(1, 2);
    actual.setWidth(3);
    ASSERT_EQ(3, actual.width());
}

TEST(size_setY, With4_ReturnSuccess)
{
    Size<int> actual(1, 2);
    actual.setHeight(4);
    ASSERT_EQ(4, actual.height());
}

TEST(size_setSize, With3And4_ReturnSuccess)
{
    Size<int> actual(1, 2);
    actual.setSize(3, 4);
    ASSERT_EQ(3, actual.width());
    ASSERT_EQ(4, actual.height());
}

TEST(size_operatorEqual, With0_0And0_0_ReturnTrue)
{
    ASSERT_EQ(Size<int>(0, 0), Size<int>(0, 0));
}

TEST(size_operatorEqual, With0_0And1_0_ReturnFalse)
{
    ASSERT_FALSE(Size<int>(0, 0) == Size<int>(1, 0));
}

TEST(size_operatorEqual, With0_0And0_1_ReturnFalse)
{
    ASSERT_FALSE(Size<int>(0, 0) == Size<int>(0, 1));
}

TEST(size_operatorEqual, With0_0And1_1_ReturnFalse)
{
    ASSERT_FALSE(Size<int>(0, 0) == Size<int>(1, 1));
}

TEST(size_operatorNotEqual, With0_0And0_0_ReturnFalse)
{
    ASSERT_FALSE(Size<int>(0, 0) != Size<int>(0, 0));
}

TEST(size_operatorNotEqual, With0_0And1_0_ReturnTrue)
{
    ASSERT_NE(Size<int>(0, 0), Size<int>(1, 0));
}

TEST(size_operatorNotEqual, With0_0And0_1_ReturnTrue)
{
    ASSERT_NE(Size<int>(0, 0), Size<int>(0, 1));
}

TEST(size_operatorNotEqual, With0_0And1_1_ReturnTrue)
{
    ASSERT_NE(Size<int>(0, 0), Size<int>(1, 1));
}
