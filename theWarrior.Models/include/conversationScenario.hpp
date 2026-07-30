#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include <boost/variant.hpp>
#include <string>
#include <vector>

namespace thewarrior::models {

using ConversationNodeId = std::string;
using ConversationScenarioId = std::string;
using StoryId = std::string;
using ItemId = std::string;

enum class ConversationNodeTransitionType {
    NextInOrder,
    Stop,
    SpecificNode
};

class ConversationNodeTransition {
 public:
    ConversationNodeTransition() = default;
    friend bool operator==(const ConversationNodeTransition &, const ConversationNodeTransition &) = default;
    static ConversationNodeTransition nextInOrder();
    static ConversationNodeTransition stop();
    static ConversationNodeTransition toNode(const ConversationNodeId &nodeId);
    ConversationNodeTransitionType getType() const;
    const ConversationNodeId &getNextNodeId() const;

 private:
    friend class boost::serialization::access;
    ConversationNodeTransitionType m_type =
        ConversationNodeTransitionType::NextInOrder;
    ConversationNodeId m_nextNodeId;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & m_type;
        ar & m_nextNodeId;
    }
};

struct ConversationDialogue {
    std::vector<std::string> lines;
    friend bool operator==(const ConversationDialogue &, const ConversationDialogue &) = default;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & lines;
    }
};

struct ConversationChoiceOption {
    std::string text;
    ConversationNodeId nextNodeId;
    friend bool operator==(const ConversationChoiceOption &, const ConversationChoiceOption &) = default;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & text;
        ar & nextNodeId;
    }
};

struct ConversationChoice {
    std::string prompt;
    std::vector<ConversationChoiceOption> options;
    friend bool operator==(const ConversationChoice &, const ConversationChoice &) = default;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & prompt;
        ar & options;
    }
};

// Actions

struct GoldReward {
    unsigned int amount = 0;
    friend bool operator==(const GoldReward &, const GoldReward &) = default;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & amount;
    }
};

struct ItemReward {
    ItemId itemId;
    unsigned int quantity = 1;
    friend bool operator==(const ItemReward &, const ItemReward &) = default;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & itemId;
        ar & quantity;
    }
};

using Reward = boost::variant<GoldReward, ItemReward>;

struct RewardAction {
    Reward reward;
    ConversationNodeTransition failureTransition =
        ConversationNodeTransition::stop();
    friend bool operator==(const RewardAction &, const RewardAction &) = default;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & reward;
        if (version >= 1) {
            ar & failureTransition;
        }
    }
};

struct ProgressStoryLineAction {
    StoryId storyId;
    std::string stageId;
    friend bool operator==(const ProgressStoryLineAction &, const ProgressStoryLineAction &) = default;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & storyId;
        ar & stageId;
    }
};

struct RestRequestedAction {
    unsigned int goldCost = 0;
    bool restoreHealth = true;
    ConversationNodeTransition failureTransition =
        ConversationNodeTransition::stop();
    friend bool operator==(const RestRequestedAction &, const RestRequestedAction &) = default;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & goldCost;
        ar & restoreHealth;
        if (version >= 1) {
            ar & failureTransition;
        }
    }
};

using ConversationAction = boost::variant<
    RewardAction,
    ProgressStoryLineAction,
    RestRequestedAction
>;

// Conversations

using ConversationNodeContent = boost::variant<
    ConversationDialogue,
    ConversationChoice,
    ConversationAction
>;

class ConversationNode {
 public:
    ConversationNode() = default;
    friend bool operator==(const ConversationNode &, const ConversationNode &) = default;
    ConversationNode(const ConversationNodeId &id,
                     const ConversationNodeContent &content,
                     const ConversationNodeTransition &transition =
                         ConversationNodeTransition::nextInOrder());
    static std::string getContentTypeName(const ConversationNodeContent &content);
    const ConversationNodeId &getId() const;
    const ConversationNodeContent &getContent() const;
    const ConversationNodeTransition &getTransition() const;
    void setId(const ConversationNodeId &id);
    void setContent(const ConversationNodeContent &content);
    void setTransition(const ConversationNodeTransition &transition);

 private:
    friend class boost::serialization::access;
    ConversationNodeId m_id;
    ConversationNodeContent m_content;
    ConversationNodeTransition m_transition;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & m_id;
        ar & m_content;
        if (version >= 1) {
            ar & m_transition;
        } else {
            m_transition = ConversationNodeTransition::nextInOrder();
        }
    }
};

class ConversationScenario {
 public:
    ConversationScenario() = default;
    friend bool operator==(const ConversationScenario &, const ConversationScenario &) = default;
    ConversationScenario(const ConversationScenarioId &id,
                         const StoryId &conditionalStoryIdCompleted,
                         const std::vector<ConversationNode> &nodes);
    static ConversationScenario fromLegacyDialogueLines(
        const std::vector<std::string> &lines);
    const ConversationScenarioId &getId() const;
    const StoryId &getConditionalStoryIdCompleted() const;
    const std::vector<ConversationNode> &getNodes() const;
    std::vector<std::string> getDialogueLines() const;
    void setId(const ConversationScenarioId &id);
    void setConditionalStoryIdCompleted(const StoryId &storyId);
    void setNodes(const std::vector<ConversationNode> &nodes);
    void setDialogueLines(const std::vector<std::string> &lines);
    void addNode(const ConversationNode &node);
    bool replaceNode(const ConversationNodeId &id,
                     const ConversationNode &node);
    bool removeNode(const ConversationNodeId &id);

 private:
    friend class boost::serialization::access;
    ConversationScenarioId m_id;
    StoryId m_conditionalStoryIdCompleted;
    std::vector<ConversationNode> m_nodes;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & m_id;
        ar & m_conditionalStoryIdCompleted;
        ar & m_nodes;
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::ConversationNode, 1)
BOOST_CLASS_VERSION(thewarrior::models::RewardAction, 1)
BOOST_CLASS_VERSION(thewarrior::models::RestRequestedAction, 1)
