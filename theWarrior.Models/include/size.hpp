#pragma once

namespace thewarrior::models {

template<typename T = int>
class Size
{
public:
    Size(T width, T height)
        : m_width(width), m_height(height) {}
    T width() const { return m_width; };
    T height() const { return m_height; };
    void setWidth(T width) { m_width = width; };
    void setHeight(T height) { m_height = height; };
    void setSize(T width, T height) {
        m_width = width;
        m_height = height;
    }
    friend bool operator==(const Size<T> &lhs, const Size<T> &rhs) {
        return lhs.m_width == rhs.m_width &&
               lhs.m_height == rhs.m_height;
    };
    friend bool operator!=(const Size<T> &lhs, const Size<T> &rhs) {
        return !(lhs == rhs);
    };
private:
    T m_width;
    T m_height;
};

} // namespace thewarrior::models
