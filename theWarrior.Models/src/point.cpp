#include "point.hpp"


Point::Point(int x, int y) 
    : m_x(x), m_y(y)
{
}

int Point::x() const
{
    return m_x;
}

int Point::y() const
{
    return m_y;
}

void Point::setX(int x) 
{
    m_x = x;
}

void Point::setY(int y) 
{
    m_y = y;
}

bool operator==(const Point &lhs, const Point &rhs)
{
    return lhs.m_x == rhs.m_x &&
           lhs.m_y == rhs.m_y;
}

bool operator!=(const Point &lhs, const Point &rhs)
{
    return !(lhs == rhs);
}