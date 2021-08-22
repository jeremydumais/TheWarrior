#pragma once

class Point
{
public:
    Point(int x, int y);
    int x() const;
    int y() const;
    void setX(int x);
    void setY(int y);
    friend bool operator==(const Point &lhs, const Point &rhs);
    friend bool operator!=(const Point &lhs, const Point &rhs);
private:
    int m_x;
    int m_y;
};