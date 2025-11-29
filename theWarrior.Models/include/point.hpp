#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

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
    friend class boost::serialization::access;
    Point() = default;  // Needed for deserialization
    T m_x;
    T m_y;
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & m_x;
        ar & m_y;
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::Point<>, 0)
