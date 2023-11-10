#pragma once

namespace thewarrior::models {

template<typename T = int>
class Point {
 public:
    Point(T x, T y)
        : m_x(x), m_y(y) {}
    T x() const { return m_x; }
    T y() const { return m_y; }
    void setX(T x) { m_x = x; }
    void setY(T y) { m_y = y; }
    friend bool operator==(const Point<T> &lhs, const Point<T> &rhs) {
        return lhs.m_x == rhs.m_x &&
               lhs.m_y == rhs.m_y;
    }
    friend bool operator!=(const Point<T> &lhs, const Point<T> &rhs) {
        return !(lhs == rhs);
    }

 private:
    T m_x;
    T m_y;
};

}  // namespace thewarrior::models
