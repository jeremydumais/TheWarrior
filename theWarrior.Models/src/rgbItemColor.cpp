#include "rgbItemColor.hpp"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <stdexcept>

using namespace boost::algorithm;

namespace thewarrior::models {

RGBItemColor::RGBItemColor(const std::string &name, const std::string &value)
    : m_name(name),
      m_value(boost::to_upper_copy(value))
{
    validateName(m_name);
    validateValue(m_value);
}

const std::string &RGBItemColor::getName() const
{
    return m_name;
}

const std::string &RGBItemColor::getValue() const
{
    return m_value;
}

void RGBItemColor::setName(const std::string &name)
{
    validateName(name);
    m_name = name;
}

void RGBItemColor::setValue(const std::string &value)
{
    validateValue(value);
    m_value = value;
}

bool operator==(const RGBItemColor &lhs, const RGBItemColor &rhs)
{
    return to_upper_copy(trim_copy(lhs.getName())) == to_upper_copy(trim_copy(rhs.getName())) &&
           to_upper_copy(trim_copy(lhs.getValue())) == to_upper_copy(trim_copy(rhs.getValue()));
}

bool operator!=(const RGBItemColor &lhs, const RGBItemColor &rhs)
{
    return !(lhs == rhs);
}

void RGBItemColor::validateName(const std::string &name) const
{
    if (trim_copy(name).empty()) {
        throw std::invalid_argument("name cannot be null or empty.") ;
    }
}

void RGBItemColor::validateValue(const std::string &value) const
{
    if (trim_copy(value).empty()) {
        throw std::invalid_argument("value cannot be null or empty.") ;
    }
    // Validate the color
    if (value.length() != 7) {
        throw std::invalid_argument("value must be in the following hexadecimal format #000000.");
    }
    size_t i = 0;
    for(const char c : value) {
        auto upperChar = toupper(c);
        if (i == 0) {
            if (upperChar != '#') {
                throw std::invalid_argument("value must start with the # char.");
            }
        }
        else if ((upperChar < 'A' || upperChar > 'F') && !(upperChar >= '0' && upperChar <= '9')) {
            throw std::invalid_argument(fmt::format("value has an invalid hexadecimal character : {}.", c));
        }
        i++;
    }
}

} // namespace thewarrior::models
