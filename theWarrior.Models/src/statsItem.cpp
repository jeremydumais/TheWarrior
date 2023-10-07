#include "statsItem.hpp"

namespace thewarrior::models {

// Used only for Boost Serialization
StatsItem::StatsItem()
    : Item(),
      m_statChanging(Stats::Vitality),
      m_gain(1.1F),
      m_limitOfOneApplied(true),
      m_durationInSecs(0) {
}

StatsItem::StatsItem(const StatsItemCreationInfo &itemInfo)
    : Item(itemInfo),
      m_statChanging(itemInfo.statChanging),
      m_gain(itemInfo.gain),
      m_limitOfOneApplied(itemInfo.limitOfOneApplied),
      m_durationInSecs(itemInfo.durationInSecs) {
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
           this->m_durationInSecs == otherStatsItem->m_durationInSecs &&
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

unsigned int StatsItem::getDurationInSecs() const {
    return m_durationInSecs;
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

void StatsItem::setDurationInSecs(unsigned int value) {
    m_durationInSecs = value;
}

}  // namespace thewarrior::models
