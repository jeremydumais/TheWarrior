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
