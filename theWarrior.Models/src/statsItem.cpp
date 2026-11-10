#include "statsItem.hpp"
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(thewarrior::models::StatsItem)

namespace thewarrior::models {

// Used only for Boost Serialization
StatsItem::StatsItem()
    : Item(),
      m_statChanging(Stats::Vitality),
      m_gain(1.1F),
      m_limitOfOneApplied(true),
      m_durationInTurn(0) {
}

StatsItem::StatsItem(const StatsItemCreationInfo &itemInfo)
    : Item(itemInfo),
      m_statChanging(itemInfo.statChanging),
      m_gain(itemInfo.gain),
      m_limitOfOneApplied(itemInfo.limitOfOneApplied),
      m_durationInTurn(itemInfo.durationInTurn) {
}

bool StatsItem::equals(const Item &other) const {
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
        return false;
    }
    const StatsItem *otherStatsItem = dynamic_cast<const StatsItem *>(&other);
    if (otherStatsItem == nullptr) {
        return false;
    }
    return this->m_statChanging == otherStatsItem->m_statChanging &&
           this->m_gain == otherStatsItem->m_gain &&
           this->m_limitOfOneApplied == otherStatsItem->m_limitOfOneApplied &&
           this->m_durationInTurn == otherStatsItem->m_durationInTurn &&
           Item::equals(other);
}

ItemType StatsItem::getType() const {
    return ItemType::StatsItem;
}

Stats StatsItem::getStatChanging() const {
    return m_statChanging;
}

float StatsItem::getGain() const {
    return m_gain;
}

bool StatsItem::getLimitOfOneApplied() const {
    return m_limitOfOneApplied;
}

unsigned int StatsItem::getDurationInTurn() const {
    return m_durationInTurn;
}

void StatsItem::setStatChanging(Stats value) {
    m_statChanging = value;
}

void StatsItem::setGain(float value) {
    m_gain = value;
}

void StatsItem::setLimitOfOneApplied(bool value) {
    m_limitOfOneApplied = value;
}

void StatsItem::setDurationInTurn(unsigned int value) {
    m_durationInTurn = value;
}

}  // namespace thewarrior::models
