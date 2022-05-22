#include "point.hpp"
#include <gtest/gtest.h>

using namespace std;

TEST(point_Contructor, With1and2_ReturnSuccess)
{
    Point actual(1, 2);
    ASSERT_EQ(1, actual.x());
    ASSERT_EQ(2, actual.y());
}

TEST(point_x, With1_Return1)
{
    Point<int> actual(1, 2);
    ASSERT_EQ(1, actual.x());
}

TEST(point_y, With2_Return2)
{
    Point<int> actual(1, 2);
    ASSERT_EQ(2, actual.y());
}

TEST(point_setX, With3_ReturnSuccess)
{
    Point<int> actual(1, 2);
    actual.setX(3);
    ASSERT_EQ(3, actual.x());
}

TEST(point_setY, With4_ReturnSuccess)
{
    Point<int> actual(1, 2);
    actual.setY(4);
    ASSERT_EQ(4, actual.y());
}

TEST(point_operatorEqual, With0_0And0_0_ReturnTrue)
{
    ASSERT_EQ(Point<int>(0, 0), Point<int>(0, 0));
}

TEST(point_operatorEqual, With0_0And1_0_ReturnFalse)
{
    ASSERT_FALSE(Point<int>(0, 0) == Point<int>(1, 0));
}

TEST(point_operatorEqual, With0_0And0_1_ReturnFalse)
{
    ASSERT_FALSE(Point<int>(0, 0) == Point<int>(0, 1));
}

TEST(point_operatorEqual, With0_0And1_1_ReturnFalse)
{
    ASSERT_FALSE(Point<int>(0, 0) == Point<int>(1, 1));
}

TEST(point_operatorNotEqual, With0_0And0_0_ReturnFalse)
{
    ASSERT_FALSE(Point<int>(0, 0) != Point<int>(0, 0));
}

TEST(point_operatorNotEqual, With0_0And1_0_ReturnTrue)
{
    ASSERT_NE(Point<int>(0, 0), Point<int>(1, 0));
}

TEST(point_operatorNotEqual, With0_0And0_1_ReturnTrue)
{
    ASSERT_NE(Point<int>(0, 0), Point<int>(0, 1));
}

TEST(point_operatorNotEqual, With0_0And1_1_ReturnTrue)
{
    ASSERT_NE(Point<int>(0, 0), Point<int>(1, 1));
}