#pragma once

#include "item.hpp"
#include "itemType.hpp"
#include "stats.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>


namespace thewarrior::models {

struct StatsItemCreationInfo : public ItemCreationInfo {
    Stats statChanging;
    float gain;
    bool limitOfOneApplied;
    unsigned int durationInSecs;
};

class StatsItem : public Item {
 public:
    StatsItem();  // Used only for Boost Serialization
    explicit StatsItem(const StatsItemCreationInfo &itemInfo);
    ~StatsItem() override = default;
    StatsItem(const StatsItem &) = default;
    StatsItem(StatsItem &&) = default;
    StatsItem &operator=(const StatsItem &) = default;
    StatsItem &operator=(StatsItem &&) = default;
    bool equals(const Item &other) const override;
    ItemType getType() const override;
    Stats getStatChanging() const;
    float getGain() const;
    bool getLimitOfOneApplied() const;
    unsigned int getDurationInSecs() const;
    void setStatChanging(Stats value);
    void setGain(float value);
    void setLimitOfOneApplied(bool value);
    void setDurationInSecs(unsigned int value);

 protected:
    friend class boost::serialization::access;
    Stats m_statChanging;
    float m_gain;
    bool m_limitOfOneApplied;
    unsigned int m_durationInSecs;
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & boost::serialization::base_object<Item>(*this);
        ar & m_statChanging;
        ar & m_gain;
        ar & m_limitOfOneApplied;
        ar & m_durationInSecs;
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::StatsItem, 0)
