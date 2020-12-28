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
    Point actual(1, 2);
    ASSERT_EQ(1, actual.x());
}

TEST(point_y, With2_Return2)
{
    Point actual(1, 2);
    ASSERT_EQ(2, actual.y());
}

TEST(point_setX, With3_ReturnSuccess)
{
    Point actual(1, 2);
    actual.setX(3);
    ASSERT_EQ(3, actual.x());
}

TEST(point_setY, With4_ReturnSuccess)
{
    Point actual(1, 2);
    actual.setY(4);
    ASSERT_EQ(4, actual.y());
}