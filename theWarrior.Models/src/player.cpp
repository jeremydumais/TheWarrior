#include "player.hpp"
#include "boost/algorithm/string.hpp"
#include <stdexcept>

Player::Player(const std::string &name)
    : m_name(name),
      m_level(1)
{
    validateName(name);
}

const std::string &Player::getName() const
{
    return m_name;
}

unsigned int Player::getLevel() const
{
    return m_level;
}

void Player::setName(const std::string &name)
{
    validateName(name);
    m_name = name;
}

void Player::setLevel(unsigned int level)
{
    m_level = level;
}

void Player::incrementLevel()
{
    m_level++;
}

void Player::validateName(const std::string &name) const
{
    if (boost::trim_copy(name).empty()) {
        throw std::invalid_argument("name cannot be null or empty.");
    }
}
