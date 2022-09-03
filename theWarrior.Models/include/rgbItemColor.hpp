#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <string>

namespace thewarrior::models {

class RGBItemColor
{
public:
    RGBItemColor(const std::string &name, const std::string &value);
    const std::string &getName() const;
    const std::string &getValue() const;
    void setName(const std::string &name);
    void setValue(const std::string &value);
    friend bool operator==(const RGBItemColor &lhs, const RGBItemColor &rhs);
    friend bool operator!=(const RGBItemColor &lhs, const RGBItemColor &rhs);
private:
    std::string m_name;
    std::string m_value;
    void validateName(const std::string &name) const;
    void validateValue(const std::string &value) const;
};

} // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::RGBItemColor, 0)
